#ifndef ALLOCATOR_STRATEGIES_H
#define ALLOCATOR_STRATEGIES_H

#include "Allocator.h"
#include "Block.h"

class FirstFit : public Allocator {
public:
    Block* findFreeBlock(Block* head, size_t size) override;
    std::string getName() const override;
};

class BestFit : public Allocator {
public:
    Block* findFreeBlock(Block* head, size_t size) override;
    std::string getName() const override;
};

class WorstFit : public Allocator {
public:
    Block* findFreeBlock(Block* head, size_t size) override;
    std::string getName() const override;
};

#endif // ALLOCATOR_STRATEGIES_H
