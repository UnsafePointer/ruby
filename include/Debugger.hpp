#pragma once
#include <vector>
#include <cstdint>

class CPU;

class Debugger {
    std::vector<uint32_t> breakpoints;
public:
    Debugger();
    ~Debugger();

    void addBreakpoint(uint32_t address);
    void removeBreakpoint(uint32_t address);
    void inspectCPU(CPU *cpu);
    void debug(CPU *cpu);
};
