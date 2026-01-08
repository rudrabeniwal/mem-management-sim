#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <deque>
#include <iostream>
#include <string>

enum class ReplacementPolicy {
    FIFO,
    LRU
};

struct CacheLine {
    bool valid;
    unsigned long long tag;
    // for LRU or FIFO tracking, we can use a timestamp or just order in the deque
    // Deque: Front is oldest (victim for FIFO/LRU if we move accessed items to back)
    
    CacheLine() : valid(false), tag(0) {} 
};

struct CacheSet {
    std::deque<CacheLine> lines; // stores ways
};

class CacheLevel {
private:
    std::string name;
    size_t size;          // total size in bytes
    size_t block_size;    // block size in bytes
    size_t associativity; // ways per set
    ReplacementPolicy policy;

    size_t num_sets;
    size_t num_lines;     // total lines
    std::vector<CacheSet> sets;

    // Stats
    size_t hits;
    size_t misses;

public:
    CacheLevel(std::string name, size_t size, size_t block_size, size_t associativity, ReplacementPolicy policy);

    // returns true on Hit, false on Miss
    bool access(unsigned long long address);

    // getters for stats
    void printStats() const;
    void resetStats();
};

#endif // CACHE_H
