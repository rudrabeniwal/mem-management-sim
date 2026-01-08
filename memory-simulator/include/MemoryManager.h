#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "Block.h"
#include "Allocator.h"
#include <iostream>
#include <vector>
#include <memory> 

class MemoryManager {
private:
    size_t total_memory_size;
    Block* memory_head; // head of the linked list of blocks
    std::unique_ptr<Allocator> allocator; // current strategy
    int next_block_id; // auto-incrementing ID for allocations
    bool is_buddy_mode; // flag for Buddy System

    // stats counters
    size_t alloc_attempts;
    size_t alloc_failures;

    // helper to merge adjacent free blocks
    void coalesce();
    // buddy specific helpers
    void coalesceBuddy();
    Block* findBuddy(Block* block);

public:
    MemoryManager();
    ~MemoryManager();

    // initialize physical memory
    void init(size_t size);

    // set the allocation strategy
    void setAllocator(const std::string& type);

    // allocation
    // returns ID of the allocated block, or -1 on failure
    int my_malloc(size_t size);

    // deallocation
    bool my_free(int block_id);

    // Debugging / Vis
    void dumpMemory() const;
    void printStats() const;
    
    // getters for integration
    Block* getHead() const { return memory_head; }
};

#endif // MEMORY_MANAGER_H
