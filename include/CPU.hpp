#pragma once
#include "Interconnect.hpp"
#include "Instruction.hpp"
#include "RegisterIndex.hpp"

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

COP0 Register Summary
cop0r0-r2   - N/A
cop0r3      - BPC - Breakpoint on execute (R/W)
cop0r4      - N/A
cop0r5      - BDA - Breakpoint on data access (R/W)
cop0r6      - JUMPDEST - Randomly memorized jump address (R)
cop0r7      - DCIC - Breakpoint control (R/W)
cop0r8      - BadVaddr - Bad Virtual Address (R)
cop0r9      - BDAM - Data Access breakpoint mask (R/W)
cop0r10     - N/A
cop0r11     - BPCM - Execute breakpoint mask (R/W)
cop0r12     - SR - System status register (R/W)
cop0r13     - CAUSE - (R)  Describes the most recently recognised exception
cop0r14     - EPC - Return Address from Trap (R)
cop0r15     - PRID - Processor ID (R)
cop0r16-r31 - Garbage
cop0r32-r63 - N/A - None such (Control regs)
*/
class CPU {
    uint32_t programCounter;
    Instruction nextInstruction;
    uint32_t registers[32];
    uint32_t outputRegisters[32];
    std::pair<RegisterIndex, uint32_t> load;
    uint32_t statusRegister;
    uint32_t highRegister;
    uint32_t lowRegister;
    const Interconnect &interconnect;
    void storeWord(uint32_t address, uint32_t value) const;
    void storeHalfWord(uint32_t address, uint16_t value) const;
    void storeByte(uint32_t address, uint8_t value) const;
    uint32_t registerAtIndex(RegisterIndex index) const;
    void setRegisterAtIndex(RegisterIndex index, uint32_t value);
    void executeNextInstruction(Instruction instruction);
    void branch(uint32_t offset);
    void operationLoadUpperImmediate(Instruction instruction);
    void operationBitwiseOrImmediate(Instruction instruction);
    void operationStoreWord(Instruction instruction) const;
    void operationShiftLeftLogical(Instruction instruction);
    void operationAddImmediateUnsigned(Instruction instruction);
    void operationJump(Instruction instruction);
    void operationBitwiseOr(Instruction Instruction);
    void operationCoprocessor0(Instruction instruction);
    void operationMoveToCoprocessor0(Instruction instruction);
    void operationBranchIfNotEqual(Instruction instruction);
    void operationAddImmediate(Instruction instruction);
    void operationLoadWord(Instruction instruction);
    void operationSetLessThanUnsigned(Instruction instruction);
    void operationAddUnsigned(Instruction instruction);
    void operationStoreHalfWord(Instruction instruction) const;
    void operationJumpAndLink(Instruction instruction);
    void operationBitwiseAndImmediate(Instruction instruction);
    void operationStoreByte(Instruction instruction) const;
    void operationJumpRegister(Instruction instruction);
    void operationLoadByte(Instruction instruction);
    void operationBranchIfEqual(Instruction instruction);
    void operationMoveFromCoprocessor0(Instruction instruction);
    void operationBitwiseAnd(Instruction instruction);
    void operationAdd(Instruction instruction);
    void operationBranchIfGreaterThanZero(Instruction instruction);
    void operationBranchIfLessThanOrEqualToZero(Instruction instruction);
    void operationLoadByteUnsigned(Instruction instruction);
    void operationJumpAndLinkRegister(Instruction instruction);
    void operationsMultipleBranchIf(Instruction instruction);
    void operationSetIfLessThanImmediate(Instruction instruction);
    void operationSubstractUnsigned(Instruction instruction);
    void operationShiftRightArithmetic(Instruction instruction);
    void operationDivision(Instruction instruction);
    void operationMoveFromLowRegister(Instruction instruction);

    uint32_t readWord(uint32_t address) const;
    uint8_t readByte(uint32_t address) const;
public:
    CPU(Interconnect &interconnect);
    ~CPU();

    void executeNextInstruction();
};
