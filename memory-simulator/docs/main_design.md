# Memory Management Simulator Design Document

## 1. Overview
This project simulates an Operating System's memory management unit, including physical memory allocation and a multilevel CPU cache hierarchy. It allows users to experiment with different allocation strategies and observe memory fragmentation and cache performance.

## 2. Memory Layout
Physical memory is modeled as a contiguous space defined by `total_memory_size`.
**Units**: All memory sizes and addresses are represented in **Bytes**.
Internally, it is managed using a **Doubly Linked List** of `Block` structures.

### Block Structure
- `id`: Unique identifier (positive for allocated, 0 for free).
- `start_address`: Offset from the beginning of physical memory.
- `size`: Size of the block in bytes.
- `is_free`: Boolean status.
- `next` / `prev`: Pointers for traversal.

Currently, the simulator separates the concept of **Heap Management** (variable size `malloc`/`free`) and **Paging** (fixed size frames). They operate as two distinct modes of simulation within the CLI, though they can conceptually share the "Physical Memory Size".

## 3. Allocation Strategies (Heap)
Implemented using the **Strategy Pattern**. The `MemoryManager` holds a pointer to an `Allocator` interface.

1.  **First Fit**: Scans the list from the beginning and selects the first free block that fits. Fast but may increase fragmentation at the start.
2.  **Best Fit**: Scans the entire list to find the smallest free block that fits. Minimizes wasted space in the split but is slower and creates tiny fragments.
3.  **Worst Fit**: Scans the entire list to find the largest free block. Leaves larger remaining chunks, potentially useful for future large allocations.
4.  **Buddy System**:
    - **Initialization**: Memory size is adjusted to the next power of 2.
    - **Allocation**: Requests are rounded up to the nearest power of 2. Large blocks are recursively split (e.g., 1024 -> 512 -> 256) until the size is reached.
    - **Deallocation**: Freed blocks check their "buddy" (adjacent block of same size). If the buddy is also free, they are coalesced into a larger block. This repeats recursively.

## 4. Cache Simulation
The simulator models a configurable Multilevel Cache (L1, L2).
Each level is simulated with:
- **Set Associativity**: Maps addresses to Sets.
- **Sets**: Collections of Cache Lines (Ways).
- **Replacement Policy**: FIFO.

Hierarchy:
CPU -> L1 -> L2 -> Main Memory

On access:
1.  Check L1. If Hit, return.
2.  If Miss, Check L2. If Hit, fill L1.
3.  If Miss, access Main Memory.

## 5. Virtual Memory (Paging)
The simulator supports a virtual memory mode enabled via `vm init`.
- **Page Size**: Fixed at 64 Bytes (matching typical cache line size for this simulation).
- **Addressing**: Addresses passed to `access` are treated as Virtual Addresses.
- **Translation**:
  - `VPN = Virtual Address / Page Size`
  - `Offset = Virtual Address % Page Size`
- **Page Table**: A Map stores valid mappings from VPN to PFN (Physical Frame Number).
- **Page Fault Handling**:
  - On access, if VPN is not in Page Table -> Page Fault.
  - Allocator finds a Free Frame (from a simple free list of PFNs).
  - If no free frames, a Victim is selected using **FIFO** replacement (tracking load order).
  - Victim is evicted (simulated), new page loaded.
  - Stats (Faults/Hits) are updated.
- **Integration**: `Virtual Address` -> `Translation (MMU)` -> `Physical Address` -> `Cache Hierarchy`.

## 6. Usage
The simulator runs an interactive CLI.

### Commands
- `init <size>`: Setup physical heap memory.
- `vm init <size>`: Initialize Virtual Memory paging system.
- `set allocator <algo>`: Change heap strategy.
- `malloc <size>`: Allocate bytes (Physical Heap Mode).
- `free <id>`: Release memory (Physical Heap Mode).
- `access <address>`: Simulate memory access. If VM is active, translates address first.
- `dump memory`: View block list (Heap).
- `vm dump`: View Page Table.
- `cache stats`: View Cache Hit/Miss rates.
- `vm stats`: View Page Fault stats.
