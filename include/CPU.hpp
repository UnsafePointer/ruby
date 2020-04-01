#pragma once
#include <memory>
#include <array>
#include "Interconnect.hpp"
#include "Instruction.hpp"
#include "Debugger.hpp"
#include "COP0.hpp"
#include "Logger.hpp"
#include "GTE.hpp"

struct LoadSlot {
    uint32_t registerIndex;
    uint32_t value;
    uint32_t previousValue;
};

/*
CPU Register Summary
Name       Alias    Common Usage
(R0)       zero     Constant (always 0) (this one isn't a real register)
R1         at       Assembler temporary (destroyed by some pseudo opcodes!)
R2-R3      v0-v1    Subroutine return values, may be changed by subroutines
R4-R7      a0-a3    Subroutine arguments, may be changed by subroutines
R8-R15     t0-t7    Temporaries, may be changed by subroutines
R16-R23    s0-s7    Static variables, must be saved by subs
R24-R25    t8-t9    Temporaries, may be changed by subroutines
R26-R27    k0-k1    Reserved for kernel (destroyed by some IRQ handlers!)
R28        gp       Global pointer (rarely used)
R29        sp       Stack pointer
R30        fp(s8)   Frame Pointer, or 9th Static variable, must be saved
R31        ra       Return address (used so by JAL,BLTZAL,BGEZAL opcodes)
-          pc       Program counter
-          hi,lo    Multiply/divide results, may be changed by subroutines
*/
class CPU {
    Logger logger;
    uint32_t programCounter;
    uint32_t jumpDestination;
    bool isBranching;
    bool runningException;
    uint32_t registers[32];
    std::array<LoadSlot, 2> loadSlots;
    uint32_t highRegister;
    uint32_t lowRegister;
    std::unique_ptr<Interconnect> &interconnect;
    std::unique_ptr<COP0> &cop0;
    Instruction currentInstruction;
    bool logBiosFunctionCalls;
    std::unique_ptr<GTE> &gte;

    void moveLoadDelaySlots();
    void loadDelaySlot(uint32_t registerIndex, uint32_t value);
    void invalidateLoadSlot(uint32_t registerIndex);

    uint32_t registerAtIndex(uint32_t index) const;
    void setRegisterAtIndex(uint32_t index, uint32_t value);

    void decodeAndExecuteInstruction(Instruction instruction);
    void branch(uint32_t offset);
    void triggerException(ExceptionType exceptionType);

    void operationLoadUpperImmediate(Instruction instruction);
    void operationBitwiseOrImmediate(Instruction instruction);
    void operationShiftLeftLogical(Instruction instruction);
    void operationAddImmediateUnsigned(Instruction instruction);
    void operationJump(Instruction instruction);
    void operationBitwiseOr(Instruction Instruction);
    void operationCoprocessor0(Instruction instruction);
    void operationMoveToCoprocessor0(Instruction instruction);
    void operationBranchIfNotEqual(Instruction instruction);
    void operationAddImmediate(Instruction instruction);
    void operationSetOnLessThanUnsigned(Instruction instruction);
    void operationAddUnsigned(Instruction instruction);
    void operationJumpAndLink(Instruction instruction);
    void operationBitwiseAndImmediate(Instruction instruction);
    void operationJumpRegister(Instruction instruction);
    void operationBranchIfEqual(Instruction instruction);
    void operationMoveFromCoprocessor0(Instruction instruction);
    void operationBitwiseAnd(Instruction instruction);
    void operationAdd(Instruction instruction);
    void operationBranchIfGreaterThanZero(Instruction instruction);
    void operationBranchIfLessThanOrEqualToZero(Instruction instruction);
    void operationJumpAndLinkRegister(Instruction instruction);
    void operationsMultipleBranchIf(Instruction instruction);
    void operationSetIfLessThanImmediate(Instruction instruction);
    void operationSubstractUnsigned(Instruction instruction);
    void operationShiftRightArithmetic(Instruction instruction);
    void operationDivision(Instruction instruction);
    void operationMoveFromLowRegister(Instruction instruction);
    void operationShiftRightLogical(Instruction instruction);
    void operationSetIfLessThanImmediateUnsigned(Instruction instruction);
    void operationDivisionUnsigned(Instruction instruction);
    void operationMoveFromHighRegister(Instruction instruction);
    void operationSetOnLessThan(Instruction instruction);
    void operationSystemCall(Instruction instruction);
    void operationMoveToLowRegister(Instruction instruction);
    void operationMoveToHighRegister(Instruction instruction);
    void operationReturnFromException(Instruction instruction);
    void operationShiftLeftLogicalVariable(Instruction instruction);
    void operationBitwiseNotOr(Instruction instruction);
    void operationShiftRightArithmeticVariable(Instruction instruction);
    void operationShiftRightLogicalVariable(Instruction instruction);
    void operationMultiplyUnsigned(Instruction instruction);
    void operationBitwiseExclusiveOr(Instruction instruction);
    void operationBreak(Instruction instruction);
    void operationMultiply(Instruction instruction);
    void operationSubstract(Instruction instruction);
    void operationBitwiseExclusiveOrImmediate(Instruction instruction);
    void operationCoprocessor1(Instruction instruction);
    void operationCoprocessor2(Instruction instruction);
    void operationCoprocessor3(Instruction instruction);

    void operationLoadWordLeft(Instruction instruction);
    void operationLoadWordRight(Instruction instruction);
    void operationStoreWordLeft(Instruction instruction);
    void operationStoreWordRight(Instruction instruction);

    void operationLoadWordCoprocessor0(Instruction instruction);
    void operationLoadWordCoprocessor1(Instruction instruction);
    void operationLoadWordCoprocessor2(Instruction instruction);
    void operationLoadWordCoprocessor3(Instruction instruction);
    void operationStoreWordCoprocessor0(Instruction instruction);
    void operationStoreWordCoprocessor1(Instruction instruction);
    void operationStoreWordCoprocessor2(Instruction instruction);
    void operationStoreWordCoprocessor3(Instruction instruction);

    void operationStoreWord(Instruction instruction);
    void operationStoreHalfWord(Instruction instruction);
    void operationStoreByte(Instruction instruction) const;

    void operationLoadWord(Instruction instruction);
    void operationLoadHalfWord(Instruction instruction);
    void operationLoadHalfWordUnsigned(Instruction instruction);
    void operationLoadByte(Instruction instruction);
    void operationLoadByteUnsigned(Instruction instruction);

    void operationMoveFromCoprocessor2(Instruction instruction);
    void operationCopyFromCoprocessor2(Instruction instruction);
    void operationMoveToCoprocessor2(Instruction instruction);
    void operationCopyToCoprocessor2(Instruction instruction);

    void operationIllegal(Instruction instruction);
public:
    CPU(LogLevel logLevel, std::unique_ptr<Interconnect> &interconnect, std::unique_ptr<COP0> &cop0, bool logBiosFunctionCalls, std::unique_ptr<GTE> &gte);
    ~CPU();

    std::unique_ptr<COP0>& cop0Ref();

    template <typename T>
    inline T load(uint32_t address) const;
    template <typename T>
    inline void store(uint32_t address, T value) const;

    bool executeNextInstruction();
    // GDB register naming and order used here:
    // r0-r31
    std::array<uint32_t, 32> getRegisters();
    // status - 32
    uint32_t getStatusRegister();
    // lo - 33
    uint32_t getLowRegister();
    // hi- 34
    uint32_t getHighRegister();
    // badvaddr - 35
    uint32_t getReturnAddressFromTrap();
    // cause - 36
    uint32_t getCauseRegister();
    // pc - 37
    uint32_t getProgramCounter();
    // r4-r7 (a0-a3)
    std::array<uint32_t, 4> getSubroutineArguments();

    void printAllRegisters();

    void setProgramCounter(uint32_t address);
    void setGlobalPointer(uint32_t address);
    void setStackPointer(uint32_t address);
    void setFramePointer(uint32_t address);
};
