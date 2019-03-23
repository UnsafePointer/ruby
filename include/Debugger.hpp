#pragma once
#include <vector>
#include <cstdint>
#include <memory>

class CPU;

class Debugger {
    static Debugger* instance;

    std::vector<uint32_t> breakpoints;
    std::vector<uint32_t> loadWatchpoints;
    std::vector<uint32_t> storeWatchpoints;

    CPU *cpu;
    Debugger();
public:
    static Debugger* getInstance();

    void setCPU(CPU *cpu);
    void addBreakpoint(uint32_t address);
    void removeBreakpoint(uint32_t address);
    void inspectCPU();
    void debug();
    void addLoadWatchpoint(uint32_t address);
    void removeLoadWatchpoint(uint32_t address);
    void inspectMemoryLoad(uint32_t address);
    void addStoreWatchpoint(uint32_t address);
    void removeStoreWatchpoint(uint32_t address);
    void inspectMemoryStore(uint32_t address);
};
