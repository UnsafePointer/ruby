#include "Debugger.hpp"
#include <algorithm>
#include "CPU.hpp"

using namespace std;

Debugger::Debugger() : breakpoints() {
}

Debugger::~Debugger() {
}

void Debugger::addBreakpoint(uint32_t address) {
    if (find(breakpoints.begin(), breakpoints.end(), address) != breakpoints.end()) {
        breakpoints.push_back(address);
    }
}

void Debugger::removeBreakpoint(uint32_t address) {
    breakpoints.erase(remove(breakpoints.begin(), breakpoints.end(), address), breakpoints.end());
}

void Debugger::inspectCPU(CPU *cpu) {
    if (find(breakpoints.begin(), breakpoints.end(), cpu->programCounter) != breakpoints.end()) {
        debug(cpu);
    }
}

void Debugger::debug(CPU *cpu) {
    return;
}
