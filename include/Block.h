#ifndef BLOCK_H
#define BLOCK_H

#include <cstddef>

struct Block {
    int id;                 // unique Identifier for the block (0 if free usually, or just track used blocks)
    size_t start_address;   // Offset from the beginning of physical memory
    size_t size;            // size of the block (Actual allocated size)
    size_t requested_size;  // size requested by user (for internal fragmentation stats)
    bool is_free;           // status
    Block* next;            // pointer to next block in the list
    Block* prev;            // pointer to previous block

    Block(int _id, size_t _start, size_t _size, bool _free = true)
        : id(_id), start_address(_start), size(_size), requested_size(0), is_free(_free), next(nullptr), prev(nullptr) {}
};

#endif // BLOCK_H
