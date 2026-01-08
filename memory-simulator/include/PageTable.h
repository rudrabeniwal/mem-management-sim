#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <vector>
#include <unordered_map>
#include <list>
#include <iostream>
#include <iomanip>

struct PageTableEntry {
    bool valid;
    int frame_number;
    // TODO: add bits for dirty, read/write
    PageTableEntry() : valid(false), frame_number(-1) {}
};

class VirtualMemoryManager {
private:
    size_t page_size;
    size_t physical_memory_size; // To know max frames
    size_t num_frames;
    
    // Page Table: Map VPN -> PTE
    // using a map to simulate a sparse or per-process page table
    std::unordered_map<int, PageTableEntry> page_table;

    // Physical Memory Tracking (Frame Allocation)
    // for simplicity, we track which physical frames are free/used
    // frame_allocation[i] = -1 if free, else contains VPN of owner (for reverse lookup during eviction)
    std::vector<int> frame_table; 
    std::list<int> free_frames;

    // Page Replacement: FIFO Queue of used frames
    // stores VPNs in order of loading
    std::list<int> present_pages_fifo; 

    // stats
    size_t page_faults;
    size_t page_hits;

public:
    VirtualMemoryManager(size_t phys_size, size_t pg_size);

    /**
     * @brief Translates Virtual Address to Physical Address.
     * Handles Page Faults if page is not in memory.
     * 
     * @param v_addr Virtual Address
     * @return long long Physical Address
     */
    unsigned long long translate(unsigned long long v_addr);

    void printStats() const;
    void printPageTable() const;

private:
    void handlePageFault(int vpn);
};

#endif // PAGE_TABLE_H
