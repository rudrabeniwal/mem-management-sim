#include "../../include/Cache.h"
#include <cmath>
#include <algorithm>
#include <iomanip>

CacheLevel::CacheLevel(std::string _name, size_t _size, size_t _block_size, size_t _associativity, ReplacementPolicy _policy)
    : name(_name), size(_size), block_size(_block_size), associativity(_associativity), policy(_policy), hits(0), misses(0) 
{
    num_lines = size / block_size;
    num_sets = num_lines / associativity;
    sets.resize(num_sets);
}

bool CacheLevel::access(unsigned long long address) {
    size_t set_index = (address / block_size) % num_sets;
    unsigned long long tag = (address / block_size) / num_sets;

    auto& set = sets[set_index];

    // Check for Hit
    for (auto it = set.lines.begin(); it != set.lines.end(); ++it) {
        if (it->valid && it->tag == tag) {
            hits++;
            // if LRU, update usage by moving to back
            if (policy == ReplacementPolicy::LRU) {
                CacheLine line = *it;
                set.lines.erase(it);
                set.lines.push_back(line); // Most recently used at back
            }
            // if FIFO, hit doesn't change order
            return true; 
        }
    }

    // misss
    misses++;
    
    // insert new line
    CacheLine new_line;
    new_line.valid = true;
    new_line.tag = tag;

    if (set.lines.size() >= associativity) {
        // evict
        // for both FIFO and LRU (with our list management), the victim is at the front
        set.lines.pop_front();
    }
    set.lines.push_back(new_line);

    return false;
}

void CacheLevel::printStats() const {
    size_t total = hits + misses;
    double hit_rate = (total > 0) ? (double)hits / total * 100.0 : 0.0;
    
    std::cout << "[" << name << "] Size: " << size << "B, Assoc: " << associativity 
              << ", Block: " << block_size << "B" << std::endl;
    std::cout << "  Hits: " << hits << "  Misses: " << misses 
              << "  Hit Rate: " << std::fixed << std::setprecision(2) << hit_rate << "%" << std::endl;
}

void CacheLevel::resetStats() {
    hits = 0;
    misses = 0;
}
