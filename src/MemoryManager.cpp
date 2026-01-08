#include "../include/MemoryManager.h"
#include "../include/AllocatorStrategies.h"
#include "buddy/BuddyUtils.h"
#include <iostream>
#include <iomanip>

MemoryManager::MemoryManager() : total_memory_size(0), memory_head(nullptr), next_block_id(1), is_buddy_mode(false), alloc_attempts(0), alloc_failures(0) {
    // Default to First Fit
    allocator = std::make_unique<FirstFit>();
}

MemoryManager::~MemoryManager() {
    Block* current = memory_head;
    while (current) {
        Block* next = current->next;
        delete current;
        current = next;
    }
}

void MemoryManager::init(size_t size) {
    if (memory_head) {
        // Clear existing memory
        Block* current = memory_head;
        while (current) {
            Block* next = current->next;
            delete current;
            current = next;
        }
    }
    
    // Reset stats
    alloc_attempts = 0;
    alloc_failures = 0;

    // For Buddy System, total size MUST be power of 2
    if (is_buddy_mode && !isPowerOf2(size)) {
        size_t new_size = nextPowerOf2(size);
        std::cout << "Warning: Buddy System requires Power of 2 memory. Resizing " << size << " -> " << new_size << std::endl;
        size = new_size;
    }

    total_memory_size = size;
    // Initial single free block covering entire memory
    memory_head = new Block(0, 0, size, true);
    next_block_id = 1;
    std::cout << "Memory initialized with " << size << " units." << std::endl;
}

void MemoryManager::setAllocator(const std::string& type) {
    is_buddy_mode = false;
    if (type == "first_fit") {
        allocator = std::make_unique<FirstFit>();
    } else if (type == "best_fit") {
        allocator = std::make_unique<BestFit>();
    } else if (type == "worst_fit") {
        allocator = std::make_unique<WorstFit>();
    } else if (type == "buddy") {
        is_buddy_mode = true;
        // Buddy uses a specific logic that overrides standard strategies, 
        // but we can default to First Fit for finding blocks before splitting
        allocator = std::make_unique<FirstFit>();
        std::cout << "Allocator set to Buddy System. (Please re-init memory if not power of 2)" << std::endl;
        return; 
    } else {
        std::cout << "Unknown allocator type. Defaulting to First Fit." << std::endl;
        allocator = std::make_unique<FirstFit>();
    }
    std::cout << "Allocator set to " << allocator->getName() << std::endl;
}

int MemoryManager::my_malloc(size_t size) {
    if (!memory_head) {
        std::cerr << "Error: Memory not initialized." << std::endl;
        return -1;
    }

    alloc_attempts++;

    // Buddy System Allocation Logic
    if (is_buddy_mode) {
        size_t req_size = nextPowerOf2(size);
        // Scan for the *smallest* free block that is >= req_size (Best Fit approach essentially)
        
        Block* current = memory_head;
        Block* target = nullptr;
        size_t min_suitable = total_memory_size + 1;
        
        while(current) {
            if (current->is_free && current->size >= req_size) {
                 if (current->size < min_suitable) {
                     min_suitable = current->size;
                     target = current;
                 }
            }
            current = current->next;
        }

        if (!target) {
            std::cerr << "Fail: No suitable block found for Buddy request " << req_size << std::endl;
            alloc_failures++;
            return -1;
        }

        // Split recursively
        while (target->size > req_size) {
            // Split in half
            size_t half_size = target->size / 2;
            size_t new_addr = target->start_address + half_size;
            
            Block* buddy = new Block(0, new_addr, half_size, true);
            
            // Link
            buddy->next = target->next;
            buddy->prev = target;
            if (target->next) target->next->prev = buddy;
            target->next = buddy;
            
            target->size = half_size; // Target becomes the left half
            // Continue loop: target is still the block we are looking at, but smaller.
        }
        
        target->is_free = false;
        target->id = next_block_id++;
        target->requested_size = size; // Track for internal fragmentation
        std::cout << "Allocated Buddy Block id=" << target->id << " (Size: " << target->size << ") at 0x" 
                  << std::hex << target->start_address << std::dec << std::endl;
        return target->id;
    }

    // Standard Allocation Logic (First/Best/Worst Fit)
    Block* target = allocator->findFreeBlock(memory_head, size);

    if (!target) {
        std::cerr << "Fail: No suitable block found for size " << size << std::endl;
        alloc_failures++;
        return -1;
    }

    if (target->size > size) {
        size_t remaining_size = target->size - size;
        size_t new_start_addr = target->start_address + size;
        
        Block* new_block = new Block(0, new_start_addr, remaining_size, true);
        
        new_block->next = target->next;
        new_block->prev = target;
        if (target->next) {
            target->next->prev = new_block;
        }
        target->next = new_block;
        
        target->size = size;
    }

    target->is_free = false;
    target->id = next_block_id++;
    target->requested_size = size;

    std::cout << "Allocated block id=" << target->id << " at address=0x" 
              << std::hex << std::uppercase << target->start_address << std::dec << std::endl;

    return target->id;
}

bool MemoryManager::my_free(int block_id) {
    Block* current = memory_head;
    while (current) {
        if (!current->is_free && current->id == block_id) {
            current->is_free = true;
            std::cout << "Block " << block_id << " freed." << std::endl;
            
            if (is_buddy_mode) {
                coalesceBuddy(); // Recursive buddy merge
            } else {
                coalesce(); // Simple merge
            }
            return true;
        }
        current = current->next;
    }
    std::cerr << "Error: Block ID " << block_id << " not found or already free." << std::endl;
    return false;
}

void MemoryManager::coalesce() {
    bool merged = false;
    Block* current = memory_head;
    
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            // Merge with next
            Block* next_block = current->next;
            
            current->size += next_block->size;
            current->next = next_block->next;
            
            if (next_block->next) {
                next_block->next->prev = current;
            }
            
            delete next_block;
            merged = true;
            // Do not advance current, check if new next is also free
        } else {
            current = current->next;
        }
    }
    if (merged) {
        std::cout << "Adjacent free blocks merged." << std::endl;
    }
}

// Buddy Coalescing: Only merge if they are valid buddies
// Buddies are blocks of same size size = 2^k
// And satisfy: address differences match buddy logic XOR
// Here, since we have a sorted list (by address), buddies are always adjacent.
// We just need to check if Address % (2 * Size) == 0 implies it's the "Left" buddy.
void MemoryManager::coalesceBuddy() {
    bool merged = true;
    while (merged) {
        merged = false;
        Block* current = memory_head;
        while (current && current->next) {
            Block* next = current->next;
            if (current->is_free && next->is_free && current->size == next->size) {
                // Must verify they are actual buddies
                // Buddy check: (Addr / Size) % 2 == 0 for left buddy
                // Actually simply: Left Buddy Address % (2 * current->size) == 0
                // Example: Size 4. Addr 0 and 4 are buddies (0 % 8 == 0).
                // Example: Size 4. Addr 4 and 8 are NOT buddies (4 % 8 != 0).
                
                size_t combined_size = current->size * 2;
                if (current->start_address % combined_size == 0) {
                    // They are buddies, merge them
                    current->size = combined_size;
                    current->next = next->next;
                    if (next->next) next->next->prev = current;
                    delete next;
                    merged = true;
                    // Restart scan or continue? Restarting is safer to catch cascading merges
                    break;
                }
            }
            current = current->next;
        }
    }
}


void MemoryManager::dumpMemory() const {
    std::cout << "\n--- Memory Dump ---" << std::endl;
    Block* current = memory_head;
    while (current) {
        std::cout << "[0x" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase 
                  << current->start_address << " - 0x" 
                  << std::setw(4) << std::setfill('0') 
                  << (current->start_address + current->size - 1) << std::dec 
                  << "] " << (current->is_free ? "FREE" : "USED")
                  << " (Size: " << current->size;
        if (!current->is_free) std::cout << ", ID: " << current->id;
        std::cout << ")" << std::endl;
        
        current = current->next;
    }
    std::cout << "-------------------\n" << std::endl;
}

void MemoryManager::printStats() const {
    size_t free_mem = 0;
    size_t used_mem = 0;
    size_t internal_frag_bytes = 0;
    size_t max_free_block = 0;
    int free_blocks = 0;

    Block* current = memory_head;
    while (current) {
        if (current->is_free) {
            free_mem += current->size;
            if (current->size > max_free_block) max_free_block = current->size;
            free_blocks++;
        } else {
            used_mem += current->size;
            // Internal fragmentation = Allocated Size - Requested Size
            if (current->size > current->requested_size) {
                internal_frag_bytes += (current->size - current->requested_size);
            }
        }
        current = current->next;
    }

    std::cout << "Total Memory: " << total_memory_size << "\n"
              << "Used Memory:  " << used_mem << " (Requested: " << (used_mem - internal_frag_bytes) << ")\n"
              << "Free Memory:  " << free_mem << "\n"
              << "Free Blocks:  " << free_blocks << "\n";
              
    // External Fragmentation: 
    // Usually defined as 1 - (Largest Free Block / Total Free Memory)
    // Or if Total Free > request but Largest Free < request.
    // Here we can show the percentage of free memory that is not in the largest block?
    // A simple metric: (Total Free - Largest Free) / Total Free * 100
    
    if (free_mem > 0) {
        double frag_percent = ((double)(free_mem - max_free_block) / free_mem) * 100.0;
        std::cout << "External Fragmentation: " << std::fixed << std::setprecision(2) << frag_percent << "%" << "\n";
    } else {
        std::cout << "External Fragmentation: 0%" << "\n";
    }

    // Internal Fragmentation
    if (used_mem > 0) {
        double int_frag_percent = ((double)internal_frag_bytes / used_mem) * 100.0;
        std::cout << "Internal Fragmentation: " << internal_frag_bytes << " bytes (" 
                  << std::fixed << std::setprecision(2) << int_frag_percent << "%)" << "\n";
    } else {
        std::cout << "Internal Fragmentation: 0 bytes" << "\n";
    }

    // Allocation Stats
    if (alloc_attempts > 0) {
        double success_rate = ((double)(alloc_attempts - alloc_failures) / alloc_attempts) * 100.0;
        std::cout << "Allocation Success Rate: " << success_rate << "% (" 
                  << (alloc_attempts - alloc_failures) << "/" << alloc_attempts << ")\n";
    } else {
        std::cout << "Allocation Success Rate: N/A\n";
    }
}
