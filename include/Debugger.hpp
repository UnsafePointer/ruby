#pragma once
#include <vector>
#include <cstdint>

class CPU;

class Debugger {
    std::vector<uint32_t> breakpoints;
    std::vector<uint32_t> loadWatchpoints;
    std::vector<uint32_t> storeWatchpoints;
public:
    Debugger();
    ~Debugger();

    void addBreakpoint(uint32_t address);
    void removeBreakpoint(uint32_t address);
    void inspectCPU(CPU *cpu);
    void debug(CPU *cpu);
    void addLoadWatchpoint(uint32_t address);
    void removeLoadWatchpoint(uint32_t address);
    void inspectMemoryLoad(CPU *cpu, uint32_t address);
    void addStoreWatchpoint(uint32_t address);
    void removeStoreWatchpoint(uint32_t address);
    void inspectMemoryStore(CPU *cpu, uint32_t address);
};
