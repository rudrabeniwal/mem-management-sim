#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "Block.h"
#include <string>

// abstract Base Class for Allocation Strategies
class Allocator {
public:
    virtual ~Allocator() = default;

    /**
     * @brief Finds a suitable free block for the requested size.
     * 
     * @param head Pointer to the head of the memory block list.
     * @param size Size of memory requested.
     * @return Block* Pointer to the suitable free block, or nullptr if none found.
     */
    virtual Block* findFreeBlock(Block* head, size_t size) = 0;

    virtual std::string getName() const = 0;
};

#endif // ALLOCATOR_H
