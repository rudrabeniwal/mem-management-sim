#include "../../include/AllocatorStrategies.h"
#include <limits>

// First Fit
Block* FirstFit::findFreeBlock(Block* head, size_t size) {
    Block* current = head;
    while (current) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

std::string FirstFit::getName() const { 
    return "First Fit"; 
}

// Best Fit
Block* BestFit::findFreeBlock(Block* head, size_t size) {
    Block* best = nullptr;
    size_t min_diff = std::numeric_limits<size_t>::max();

    Block* current = head;
    while (current) {
        if (current->is_free && current->size >= size) {
            size_t diff = current->size - size;
            if (diff < min_diff) {
                min_diff = diff;
                best = current;
            }
        }
        current = current->next;
    }
    return best;
}

std::string BestFit::getName() const { 
    return "Best Fit"; 
}

// Worst Fit
Block* WorstFit::findFreeBlock(Block* head, size_t size) {
    Block* worst = nullptr;
    size_t max_size = 0;

    Block* current = head;
    while (current) {
        if (current->is_free && current->size >= size) {
            if (current->size > max_size) {
                max_size = current->size;
                worst = current;
            }
        }
        current = current->next;
    }
    return worst;
}

std::string WorstFit::getName() const { 
    return "Worst Fit"; 
}
