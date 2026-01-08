#include "../include/PageTable.h"

VirtualMemoryManager::VirtualMemoryManager(size_t phys_size, size_t pg_size)
    : page_size(pg_size), physical_memory_size(phys_size), page_faults(0), page_hits(0) 
{
    num_frames = physical_memory_size / page_size;
    frame_table.resize(num_frames, -1); // initialize all frames as free (-1)

    // add all frames to free list
    for (size_t i = 0; i < num_frames; ++i) {
        free_frames.push_back(i);
    }
    
    std::cout << "Virtual Memory Initialized: " << num_frames << " Frames of size " << page_size << std::endl;
}

unsigned long long VirtualMemoryManager::translate(unsigned long long v_addr) {
    int vpn = v_addr / page_size;
    int offset = v_addr % page_size;

    // check Page Table
    if (page_table.find(vpn) != page_table.end() && page_table[vpn].valid) {
        // Hit
        page_hits++;
        int pfn = page_table[vpn].frame_number;
        // move to back? No, that's LRU. FIFO records order of *loading*, not access.
        return (unsigned long long)pfn * page_size + offset;
    }

    // Misss -> Page Fault
    std::cout << "  > Page Fault for VPN " << vpn << std::endl;
    page_faults++;
    handlePageFault(vpn);

    // retry translation known to be valid now
    int pfn = page_table[vpn].frame_number;
    return (unsigned long long)pfn * page_size + offset;
}

void VirtualMemoryManager::handlePageFault(int vpn) {
    int frame_idx = -1;

    // 1. Check if there is a free frame
    if (!free_frames.empty()) {
        frame_idx = free_frames.front();
        free_frames.pop_front();
    } 
    else {
        // 2. No free frames -> Eviction (FIFO)
        if (present_pages_fifo.empty()) {
            std::cerr << "Criticial Error: No pages to evict but no free frames?" << std::endl;
            return;
        }

        // Simulate Disk Access Latency (Symbolic)
        std::cout << "  [Disk Access] Saving victim page to disk... (Latency simulated)" << std::endl;

        // Pop the first loaded page
        int victim_vpn = present_pages_fifo.front();
        present_pages_fifo.pop_front();

        // Invalidate victim in Page Table
        if (page_table.count(victim_vpn)) {
            page_table[victim_vpn].valid = false;
            frame_idx = page_table[victim_vpn].frame_number;
            std::cout << "  > Evicting VPN " << victim_vpn << " from Frame " << frame_idx << std::endl;
        }
    }

    // 3. Load new page into frame
    PageTableEntry pte;
    pte.valid = true;
    pte.frame_number = frame_idx;
    
    page_table[vpn] = pte;
    frame_table[frame_idx] = vpn; // record owner
    present_pages_fifo.push_back(vpn); // add to FIFO queue
    
    // Simulate Disk Access Latency for Loading
    std::cout << "  [Disk Access] Loading page " << vpn << " from disk... (Latency simulated)" << std::endl;

    std::cout << "  > Loaded VPN " << vpn << " into Frame " << frame_idx << std::endl;
}

void VirtualMemoryManager::printStats() const {
    size_t total = page_faults + page_hits;
    double fault_rate = (total > 0) ? (double)page_faults / total * 100.0 : 0.0;

    std::cout << "Virtual Memory Statistics:\n"
              << "  Page Hits:   " << page_hits << "\n"
              << "  Page Faults: " << page_faults << "\n"
              << "  Fault Rate:  " << std::fixed << std::setprecision(2) << fault_rate << "%\n";
}

void VirtualMemoryManager::printPageTable() const {
    std::cout << "Page Table Dump (Valid Entries):\n";
    std::cout << "  VPN   | Frame | Valid \n";
    std::cout << "  ------|-------|-------\n";
    for (const auto& pair : page_table) {
        if (pair.second.valid) {
            std::cout << "  " << std::setw(5) << pair.first << " | " 
                      << std::setw(5) << pair.second.frame_number << " | " 
                      << "YES" << "\n";
        }
    }
}
