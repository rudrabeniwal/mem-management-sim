# Memory Management Simulator

A complete simulation of an Operating System's Memory Management Unit (MMU), implemented in C++.  
This project demonstrates **Dynamic Memory Allocation**, **Multilevel Caching**, and **Virtual Memory Paging**.

## Features

1.  **Physical Memory Allocator**
    *   **Strategies**: `First Fit`, `Best Fit`, `Worst Fit`.
    *   **Buddy System**: Recursive splitting and coalescing for power-of-2 blocks.
    *   **Coalescing**: Automatic merging of adjacent free blocks.

2.  **Virtual Memory (Paging)**
    *   Translates **Virtual Addresses** to **Physical Addresses**.
    *   Simulates **Page Faults**.
    *   **FIFO** Page Replacement policy.
    *   Configurable physical memory limits (frames).

3.  **Multilevel Cache**
    *   **L1 Cache**: 1KB, 2-way Set Associative.
    *   **L2 Cache**: 4KB, 4-way Set Associative.
    *   **Hierarchy**: CPU -> L1 -> L2 -> Main RAM.
    *   Connects seamlessly with the Virtual Memory system (physically addressed cache).

## Build Instructions

### Prerequisites
*   C++ Compiler (g++ or clang++) supporting C++17.
*   Make (optional).

### Building with Make
```bash
make
```

### Manual Build
```bash
g++ -std=c++17 -I./include src/main.cpp src/MemoryManager.cpp src/cache/Cache.cpp src/virtual_memory/PageTable.cpp -o memsim
```

## Usage

Run the simulator:
```bash
./memsim
```

### 1. Basic Allocation Mode
Simulate standard `malloc`/`free` behavior.
```bash
> init 1024              # Init 1024 bytes
> set allocator best_fit # Choose strategy
> malloc 200             # Alloc 200 bytes
> malloc 50
> free 1                 # Free block ID 1
> dump memory            # View heap map
```

### 2. Buddy System Mode
Simulate the Buddy Allocation algorithm.
```bash
> init 1024              # Must be power of 2
> set allocator buddy
> malloc 100             # Rounds to 128, splits blocks
> free 1
```

### 3. Virtual Memory & Cache Simulation
Simulate CPU memory access through the full hierarchy.
```bash
> vm init 256            # Init VM with 256 bytes of Physical RAM (4 Frames)
> access 0x0040          # Access Virtual Addr 0x40
                         # -> MMU Translates (Page Fault if needed)
                         # -> Physical Addr sent to Cache
                         # -> L1/L2 Hit/Miss logic
> vm stats               # Check Fault rates
> cache stats            # Check Hit rates
```

## Project Structure
*   `src/`: Source code.
*   `include/`: Header files.
*   `tests/`: Sample test scripts.
*   `docs/`: Detailed design documentation.
