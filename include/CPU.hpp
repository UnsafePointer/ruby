#pragma once
#include "Interconnect.hpp"
#include "Instruction.hpp"
#include "RegisterIndex.hpp"

class CPU {
    uint32_t programCounter;
    Instruction nextInstruction;
    uint32_t registers[32];
    const Interconnect &interconnect;
    void storeWord(uint32_t address, uint32_t value) const;
    uint32_t registerAtIndex(RegisterIndex index) const;
    void setRegisterAtIndex(RegisterIndex index, uint32_t value);
    void executeNextInstruction(Instruction instruction);
    void operationLoadUpperImmediate(Instruction instruction);
    void operationBitwiseOrImmediate(Instruction instruction);
    void operationStoreWord(Instruction instruction) const;
    void operationShiftLeftLogical(Instruction instruction);
    void operationAddImmediateUnsigned(Instruction instruction);
    void operationJump(Instruction instruction);
    void operationOr(Instruction Instruction);
public:
    CPU(Interconnect &interconnect);
    ~CPU();

    uint32_t readWord(uint32_t address) const;
    void executeNextInstruction();
};
