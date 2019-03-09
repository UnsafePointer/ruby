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

void Debugger::addLoadWatchpoint(uint32_t address) {
    if (find(loadWatchpoints.begin(), loadWatchpoints.end(), address) != loadWatchpoints.end()) {
        loadWatchpoints.push_back(address);
    }
}

void Debugger::removeLoadWatchpoint(uint32_t address) {
    loadWatchpoints.erase(remove(loadWatchpoints.begin(), loadWatchpoints.end(), address), loadWatchpoints.end());
}

void Debugger::inspectMemoryLoad(CPU *cpu, uint32_t address) {
    if (find(loadWatchpoints.begin(), loadWatchpoints.end(), address) != loadWatchpoints.end()) {
        debug(cpu);
    }
}

void Debugger::addStoreWatchpoint(uint32_t address) {
    if (find(storeWatchpoints.begin(), storeWatchpoints.end(), address) != storeWatchpoints.end()) {
        storeWatchpoints.push_back(address);
    }
}

void Debugger::removeStoreWatchpoint(uint32_t address) {
    storeWatchpoints.erase(remove(storeWatchpoints.begin(), storeWatchpoints.end(), address), storeWatchpoints.end());
}

void Debugger::inspectMemoryStore(CPU *cpu, uint32_t address) {
    if (find(storeWatchpoints.begin(), storeWatchpoints.end(), address) != storeWatchpoints.end()) {
        debug(cpu);
    }
}

void Debugger::debug(CPU *cpu) {
    return;
}
