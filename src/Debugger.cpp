#include "Debugger.hpp"
#include <algorithm>
#include "CPU.hpp"
#ifdef HANA
#include "libHana.h"
#endif

using namespace std;

Debugger::Debugger() : breakpoints(), loadWatchpoints(), storeWatchpoints() {
}

Debugger* Debugger::instance = nullptr;

Debugger* Debugger::getInstance() {
    if (instance == nullptr) {
        instance = new Debugger();
    }
    return instance;
}

void Debugger::setCPU(CPU *cpu) {
    this->cpu = cpu;
}

CPU* Debugger::getCPU() {
    return cpu;
}

void Debugger::addBreakpoint(uint32_t address) {
    if (find(breakpoints.begin(), breakpoints.end(), address) != breakpoints.end()) {
        breakpoints.push_back(address);
    }
}

void Debugger::removeBreakpoint(uint32_t address) {
    breakpoints.erase(remove(breakpoints.begin(), breakpoints.end(), address), breakpoints.end());
}

void Debugger::inspectCPU() {
    if (find(breakpoints.begin(), breakpoints.end(), cpu->getProgramCounter()) != breakpoints.end()) {
        debug();
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

void Debugger::inspectMemoryLoad(uint32_t address) {
    if (find(loadWatchpoints.begin(), loadWatchpoints.end(), address) != loadWatchpoints.end()) {
        debug();
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

void Debugger::inspectMemoryStore(uint32_t address) {
    if (find(storeWatchpoints.begin(), storeWatchpoints.end(), address) != storeWatchpoints.end()) {
        debug();
    }
}

extern "C" uint32_t* globalRegisters() {
    Debugger *debugger = Debugger::getInstance();
    debugger->getCPU()->printAllRegisters();
    uint32_t *regs = (uint32_t *) malloc(sizeof(uint32_t) * 37);
    array<uint32_t, 32> cpuRegisters = debugger->getCPU()->getRegisters();
    for (uint8_t i = 0; i < cpuRegisters.size(); i++) {
        regs[i] = cpuRegisters[i];
    }
    regs[32] = debugger->getCPU()->getStatusRegister();
    regs[33] = debugger->getCPU()->getLowRegister();
    regs[34] = debugger->getCPU()->getHighRegister();
    regs[35] = debugger->getCPU()->getReturnAddressFromTrap();
    regs[36] = debugger->getCPU()->getCauseRegister();
    regs[37] = debugger->getCPU()->getProgramCounter();
    return regs;
}

void Debugger::debug() {
#ifdef HANA
    SetGlobalRegistersCallback(&globalRegisters);
    StartDebugServer(1111);
#endif
    return;
}
