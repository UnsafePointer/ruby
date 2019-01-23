#pragma once
#include "Interconnect.hpp"

class CPU {
    uint32_t programCounter;
    const Interconnect &interconnect;

    void executeNextInstruction(uint32_t instruction);
public:
    CPU(Interconnect &interconnect);
    ~CPU();

    uint32_t readWord(uint32_t address) const;
    void executeNext();
};
