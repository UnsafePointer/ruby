#pragma once
#include "Interconnect.hpp"
#include "Instruction.hpp"

class CPU {
    uint32_t programCounter;
    uint32_t registers[32];
    const Interconnect &interconnect;
    void storeWord(uint32_t address, uint32_t value) const;
    uint32_t registerAtIndex(uint8_t index) const;
    void setRegisterAtIndex(uint8_t index, uint32_t value);
    void executeNextInstruction(Instruction instruction);
    void operationLoadUpperImmediate(Instruction instruction);
    void operationBitwiseOrImmediate(Instruction instruction);
    void operationStoreWord(Instruction instruction) const;
public:
    CPU(Interconnect &interconnect);
    ~CPU();

    uint32_t readWord(uint32_t address) const;
    void executeNext();
};
