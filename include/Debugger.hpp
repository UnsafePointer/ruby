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
    bool stopped;
    bool attached;
    bool step;
    Debugger();
    ~Debugger();
public:
    static Debugger* getInstance();
    static void removeInstance();

    void setCPU(CPU *cpu);
    CPU* getCPU();
    bool isAttached();
    bool isStopped();
    bool shouldStep();
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
    void continueProgram();
    void prepareStep();
    void doStep();
};
