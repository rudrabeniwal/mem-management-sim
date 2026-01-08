#include "../include/MemoryManager.h"
#include "../include/Cache.h"
#include "../include/PageTable.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <memory> 

void printHelp() {
    std::cout << "Available commands:\n"
              << "  init <size>             Initialize memory with size\n"
              << "  set allocator <algo>    Set switch (first_fit, best_fit, worst_fit, buddy)\n"
              << "  malloc <size>           Allocate memory\n"
              << "  free <id>               Free memory block by ID\n"
              << "  dump memory             Show memory map\n"
              << "  stats                   Show statistics\n"
              << "  \n"
              << "  Cache Commands:\n"
              << "  cache init              Initialize L1 (1KB, 64B, 2-way) and L2 (4KB, 64B, 4-way)\n"
              << "  cache stats             Show cache hit/miss stats\n"
              << "  \n"
              << "  Virtual Memory Commands:\n"
              << "  vm init <phys_size>     Init VM with Total Physical Size (Page Size fixed at 64B)\n"
              << "  vm stats                Show Page Fault stats\n"
              << "  vm dump                 Show Page Table\n"
              << "  access <address>        Read address (translates Virtual -> Physical if VM active, then Cache)\n"
              << "  \n"
              << "  exit                    Exit simulator\n";
}

int main() {
    MemoryManager memManager;
    
    // Simple 2-level cache hierarchy
    std::unique_ptr<CacheLevel> l1;
    std::unique_ptr<CacheLevel> l2;
    
    // Virtual Memory
    std::unique_ptr<VirtualMemoryManager> vm;
    bool use_vm = false;

    // Default init for cache
    l1 = std::make_unique<CacheLevel>("L1 Cache", 1024, 64, 2, ReplacementPolicy::FIFO);
    l2 = std::make_unique<CacheLevel>("L2 Cache", 4096, 64, 4, ReplacementPolicy::FIFO);

    std::string line;
    
    std::cout << "Memory Management Simulator\nType 'help' for commands.\n";

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string command;
        ss >> command;

        if (command == "exit" || command == "quit") {
            break;
        } else if (command == "help") {
            printHelp();
        } else if (command == "vm") {
            std::string sub;
            ss >> sub;
            if (sub == "init") {
                size_t phys_size;
                if (ss >> phys_size) {
                    // Page size matched to Cache Block Size for simplicity (64B)
                    vm = std::make_unique<VirtualMemoryManager>(phys_size, 64);
                    use_vm = true;
                } else {
                    std::cout << "Usage: vm init <physical_memory_size>\n";
                }
            } else if (sub == "stats") {
                if(vm) vm->printStats();
                else std::cout << "VM not initialized.\n";
            } else if (sub == "dump") {
                if(vm) vm->printPageTable();
                else std::cout << "VM not initialized.\n";
            }
        }
        else if (command == "cache") {
            std::string sub;
            ss >> sub;
            if (sub == "init") {
                l1 = std::make_unique<CacheLevel>("L1 Cache", 1024, 64, 2, ReplacementPolicy::FIFO);
                l2 = std::make_unique<CacheLevel>("L2 Cache", 4096, 64, 4, ReplacementPolicy::FIFO);
                std::cout << "Caches initialized.\n";
            } else if (sub == "stats") {
                if (l1) l1->printStats();
                if (l2) l2->printStats();
            } else {
                std::cout << "Usage: cache <init|stats>\n";
            }
        } else if (command == "access") {
            std::string addrStr;
            if (ss >> addrStr) {
                unsigned long long v_addr;
                unsigned long long p_addr;
                try {
                    v_addr = std::stoull(addrStr, nullptr, 0);
                } catch (...) {
                    std::cout << "Invalid address format.\n";
                    continue;
                }

                if (use_vm && vm) {
                    std::cout << "Virtual Address: 0x" << std::hex << v_addr << std::dec << "\n";
                    p_addr = vm->translate(v_addr);
                    std::cout << "Translated to Physical Address: 0x" << std::hex << p_addr << std::dec << "\n";
                } else {
                    p_addr = v_addr;
                    std::cout << "Physical Address: 0x" << std::hex << p_addr << std::dec << " (VM disabled)\n";
                }
                
                // L1 Access
                bool hit1 = l1->access(p_addr);
                if (hit1) {
                    std::cout << "L1 Cache HIT\n";
                } else {
                    std::cout << "L1 Cache MISS -> Accessing L2...\n";
                    // L2 Access
                    bool hit2 = l2->access(p_addr);
                    if (hit2) {
                        std::cout << "L2 Cache HIT\n";
                    } else {
                        std::cout << "L2 Cache MISS -> Main Memory Access\n";
                    }
                }

            } else {
                std::cout << "Usage: access <address>\n";
            }
        } else if (command == "init") {
            size_t size;
            if (ss >> size) {
                memManager.init(size);
            } else {
                std::cout << "Usage: init <size>\n";
            }
        } else if (command == "set") {
            std::string sub, algo;
            ss >> sub;
            if (sub == "allocator") {
                if (ss >> algo) {
                    memManager.setAllocator(algo);
                } else {
                    std::cout << "Usage: set allocator <first_fit|best_fit|worst_fit|buddy>\n";
                }
            } else {
                std::cout << "Unknown set command.\n";
            }
        } else if (command == "malloc") {
            size_t size;
            if (ss >> size) {
                memManager.my_malloc(size);
            } else {
                std::cout << "Usage: malloc <size>\n";
            }
        } else if (command == "free") {
            int id;
            if (ss >> id) {
                memManager.my_free(id);
            } else {
                std::cout << "Usage: free <id>\n";
            }
        } else if (command == "dump") {
            // consume "memory" optional word
            std::string sub;
            if (ss >> sub && sub != "memory") {
                 // ignore or handle error? just lenient.
            }
            memManager.dumpMemory();
        } else if (command == "stats") {
            memManager.printStats();
        } else {
            std::cout << "Unknown command: " << command << "\n";
        }
    }

    return 0;
}
