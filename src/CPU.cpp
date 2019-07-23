#include "CPU.hpp"
#include <algorithm>
#include <tuple>
#include <iomanip>
#include "CPU.tcc"
#include "Output.hpp"
#include "TestRunner.hpp"

using namespace std;

CPU::CPU(unique_ptr<Interconnect> &interconnect, unique_ptr<COP0> &cop0) : programCounter(0xbfc00000),
             jumpDestination(0),
             isBranching(false),
             runningException(false),
             loadSlots(),
             highRegister(0),
             lowRegister(0),
             interconnect(interconnect),
             cop0(cop0),
             currentInstruction(Instruction(0x0))
{
    fill_n(registers, 32, 0);
}

CPU::~CPU() {}

unique_ptr<COP0>& CPU::cop0Ref() {
    return cop0;
}

void CPU::setProgramCounter(uint32_t address) {
    programCounter = address;
}

void CPU::setGlobalPointer(uint32_t address) {
    registers[28] = address;
}

void CPU::setStackPointer(uint32_t address) {
    registers[29] = address;
}

void CPU::setFramePointer(uint32_t address) {
    registers[30] = address;
}

std::array<uint32_t, 32> CPU::getRegisters() {
    array<uint32_t, 32> regs;
    copy(begin(registers), end(registers), begin(regs));
    return regs;
}

uint32_t CPU::getStatusRegister() {
    return cop0->status.value;
}

uint32_t CPU::getLowRegister() {
    return lowRegister;
}

uint32_t CPU::getHighRegister() {
    return highRegister;
}

uint32_t CPU::getReturnAddressFromTrap() {
    return cop0->returnAddressFromTrap;
}

uint32_t CPU::getCauseRegister() {
    return cop0->cause.value;
}

uint32_t CPU::getProgramCounter() {
    return programCounter;
}

array<uint32_t, 4> CPU::getSubroutineArguments() {
    uint32_t subroutineArguments[4];
    memcpy(subroutineArguments, &registers[4], 4*sizeof(*registers));
    array<uint32_t, 4> args;
    copy(begin(subroutineArguments), end(subroutineArguments), begin(args));
    return args;
}

void CPU::printAllRegisters() {
    printWarning("CPU Registers: ");
    for (uint i = 0; i < 32; i++) {
        printWarning("r%02d: %#x", i, registers[i]);
    }
    printWarning("status: %#x", getStatusRegister());
    printWarning("lo: %#x", lowRegister);
    printWarning("hi: %#x", highRegister);
    printWarning("badvaddr: %#x", getReturnAddressFromTrap());
    printWarning("cause: %#x", getCauseRegister());
    printWarning("pc: %#x", programCounter);
}

bool CPU::executeNextInstruction() {
    if (cop0->areInterruptsPending()) {
        triggerException(ExceptionType::Interrupt);
    }
    if (cop0->breakPointControl & (1 << 24) && programCounter == cop0->breakPointOnExecute) {
        cop0->breakPointControl  &= ~(1 << 24);
        return false;
    }

    Debugger *debugger = Debugger::getInstance();
    debugger->inspectCPU();

    currentInstruction = Instruction(load<uint32_t>(programCounter));

    bool isBranchingCycle = isBranching;

    decodeAndExecuteInstruction(currentInstruction);

    moveLoadDelaySlots();

    if (runningException) {
        runningException = false;
        return true;
    }

    if (isBranchingCycle) {
        programCounter = jumpDestination & 0xfffffffc;
        jumpDestination = 0;
        isBranching = false;
    } else {
        programCounter += 4;
    }

    return true;
}

void CPU::loadDelaySlot(uint32_t registerIndex, uint32_t value) {
    if (registerIndex == 0) {
        return;
    }
    if (registerIndex == loadSlots[0].registerIndex) {
        loadSlots[0].registerIndex = 0;
    }

    loadSlots[1].registerIndex = registerIndex;
    loadSlots[1].value = value;
    loadSlots[1].previousValue = registers[registerIndex];
}

void CPU::moveLoadDelaySlots() {
    if (loadSlots[0].registerIndex != 0) {
        if (registers[loadSlots[0].registerIndex] == loadSlots[0].previousValue) {
            registers[loadSlots[0].registerIndex] = loadSlots[0].value;
        }
    }

    loadSlots[0] = loadSlots[1];
    loadSlots[1].registerIndex = 0;
}

uint32_t CPU::registerAtIndex(uint32_t index) const {
    return registers[index];
}

void CPU::invalidateLoadSlot(uint32_t registerIndex) {
    if (loadSlots[0].registerIndex == registerIndex) {
        loadSlots[0].registerIndex = 0;
    }
}

void CPU::setRegisterAtIndex(uint32_t index, uint32_t value) {
    registers[index] = value;

    // Make sure R0 is always 0
    registers[0] = 0;

    invalidateLoadSlot(index);
}

void CPU::decodeAndExecuteInstruction(Instruction instruction) {
    switch (instruction.funct) {
        case 0b000000: {
            switch (instruction.subfunct) {
                case 0b000000: {
                    operationShiftLeftLogical(instruction);
                    break;
                }
                case 0b000010: {
                    operationShiftRightLogical(instruction);
                    break;
                }
                case 0b000011: {
                    operationShiftRightArithmetic(instruction);
                    break;
                }
                case 0b000100: {
                    operationShiftLeftLogicalVariable(instruction);
                    break;
                }
                case 0b000110: {
                    operationShiftRightLogicalVariable(instruction);
                    break;
                }
                case 0b000111: {
                    operationShiftRightArithmeticVariable(instruction);
                    break;
                }
                case 0b001000: {
                    operationJumpRegister(instruction);
                    break;
                }
                case 0b001001: {
                    operationJumpAndLinkRegister(instruction);
                    break;
                }
                case 0b001100: {
                    operationSystemCall(instruction);
                    break;
                }
                case 0b001101: {
                    operationBreak(instruction);
                    break;
                }
                case 0b010000: {
                    operationMoveFromHighRegister(instruction);
                    break;
                }
                case 0b010001: {
                    operationMoveToHighRegister(instruction);
                    break;
                }
                case 0b010010: {
                    operationMoveFromLowRegister(instruction);
                    break;
                }
                case 0b010011: {
                    operationMoveToLowRegister(instruction);
                    break;
                }
                case 0b011000: {
                    operationMultiply(instruction);
                    break;
                }
                case 0b011001: {
                    operationMultiplyUnsigned(instruction);
                    break;
                }
                case 0b011010: {
                    operationDivision(instruction);
                    break;
                }
                case 0b011011: {
                    operationDivisionUnsigned(instruction);
                    break;
                }
                case 0b100000: {
                    operationAdd(instruction);
                    break;
                }
                case 0b100001: {
                    operationAddUnsigned(instruction);
                    break;
                }
                case 0b100010: {
                    operationSubstract(instruction);
                    break;
                }
                case 0b100011: {
                    operationSubstractUnsigned(instruction);
                    break;
                }
                case 0b100100: {
                    operationBitwiseAnd(instruction);
                    break;
                }
                case 0b100101: {
                    operationBitwiseOr(instruction);
                    break;
                }
                case 0b100110: {
                    operationBitwiseExclusiveOr(instruction);
                    break;
                }
                case 0b100111: {
                    operationBitwiseNotOr(instruction);
                    break;
                }
                case 0b101010: {
                    operationSetOnLessThan(instruction);
                    break;
                }
                case 0b101011: {
                    operationSetOnLessThanUnsigned(instruction);
                    break;
                }
                default: {
                    operationIllegal(instruction);
                    break;
                }
            }
            break;
        }
        case 0b000001: {
            operationsMultipleBranchIf(instruction);
            break;
        }
        case 0b000010: {
            operationJump(instruction);
            break;
        }
        case 0b000011: {
            operationJumpAndLink(instruction);
            break;
        }
        case 0b000100: {
            operationBranchIfEqual(instruction);
            break;
        }
        case 0b000101: {
            operationBranchIfNotEqual(instruction);
            break;
        }
        case 0b000110: {
            operationBranchIfLessThanOrEqualToZero(instruction);
            break;
        }
        case 0b000111: {
            operationBranchIfGreaterThanZero(instruction);
            break;
        }
        case 0b001000: {
            operationAddImmediate(instruction);
            break;
        }
        case 0b01001: {
            operationAddImmediateUnsigned(instruction);
            break;
        }
        case 0b001010: {
            operationSetIfLessThanImmediate(instruction);
            break;
        }
        case 0b001011: {
            operationSetIfLessThanImmediateUnsigned(instruction);
            break;
        }
        case 0b001100: {
            operationBitwiseAndImmediate(instruction);
            break;
        }
        case 0b001101: {
            operationBitwiseOrImmediate(instruction);
            break;
        }
        case 0b001110: {
            operationBitwiseExclusiveOrImmediate(instruction);
            break;
        }
        case 0b001111: {
            operationLoadUpperImmediate(instruction);
            break;
        }
        case 0b010000: {
            operationCoprocessor0(instruction);
            break;
        }
        case 0b010001: {
            operationCoprocessor1(instruction);
            break;
        }
        case 0b010010: {
            operationCoprocessor2(instruction);
            break;
        }
        case 0b010011: {
            operationCoprocessor3(instruction);
            break;
        }
        case 0b100000: {
            operationLoadByte(instruction);
            break;
        }
        case 0b100001: {
            operationLoadHalfWord(instruction);
            break;
        }
        case 0b100010: {
            operationLoadWordLeft(instruction);
            break;
        }
        case 0b100011: {
            operationLoadWord(instruction);
            break;
        }
        case 0b100100: {
            operationLoadByteUnsigned(instruction);
            break;
        }
        case 0b100101: {
            operationLoadHalfWordUnsigned(instruction);
            break;
        }
        case 0b100110: {
            operationLoadWordRight(instruction);
            break;
        }
        case 0b101000: {
            operationStoreByte(instruction);
            break;
        }
        case 0b101001: {
            operationStoreHalfWord(instruction);
            break;
        }
        case 0b101010: {
            operationStoreWordLeft(instruction);
            break;
        }
        case 0b101011: {
            operationStoreWord(instruction);
            break;
        }
        case 0b101110: {
            operationStoreWordRight(instruction);
            break;
        }
        case 0b110000: {
            operationLoadWordCoprocessor0(instruction);
            break;
        }
        case 0b110001: {
            operationLoadWordCoprocessor1(instruction);
            break;
        }
        case 0b110010: {
            operationLoadWordCoprocessor2(instruction);
            break;
        }
        case 0b110011: {
            operationLoadWordCoprocessor3(instruction);
            break;
        }
        case 0b111000: {
            operationStoreWordCoprocessor0(instruction);
            break;
        }
        case 0b111001: {
            operationStoreWordCoprocessor1(instruction);
            break;
        }
        case 0b111010: {
            operationStoreWordCoprocessor2(instruction);
            break;
        }
        case 0b111011: {
            operationStoreWordCoprocessor3(instruction);
            break;
        }
        default: {
            operationIllegal(instruction);
            break;
        }
    }
}

void CPU::operationShiftLeftLogical(Instruction instruction) {
    uint32_t imm = instruction.shiftimm;
    uint32_t rt = instruction.rt;
    uint32_t rd = instruction.rd;

    uint32_t value = registerAtIndex(rt) << imm;
    setRegisterAtIndex(rd, value);
}

void CPU::operationShiftRightLogical(Instruction instruction) {
    uint32_t imm = instruction.shiftimm;
    uint32_t rt = instruction.rt;
    uint32_t rd = instruction.rd;

    uint32_t value = registerAtIndex(rt) >> imm;
    setRegisterAtIndex(rd, value);
}

void CPU::operationShiftRightArithmetic(Instruction instruction) {
    uint32_t imm = instruction.shiftimm;
    uint32_t rt = instruction.rt;
    uint32_t rd = instruction.rd;

    uint32_t value = ((int32_t)registerAtIndex(rt)) >> imm;
    setRegisterAtIndex(rd, value);
}

void CPU::operationShiftLeftLogicalVariable(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint32_t value = registerAtIndex(rt) << (registerAtIndex(rs) & 0x1f);
    setRegisterAtIndex(rd, value);
}

void CPU::operationShiftRightLogicalVariable(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint32_t value = registerAtIndex(rt) >> (registerAtIndex(rs) & 0x1f);
    setRegisterAtIndex(rd, value);
}

void CPU::operationShiftRightArithmeticVariable(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint32_t value = ((int32_t)registerAtIndex(rt)) >> (registerAtIndex(rs) & 0x1f);
    setRegisterAtIndex(rd, value);
}

void CPU::operationJumpRegister(Instruction instruction) {
    uint32_t rs = instruction.rs;
    uint32_t address = registerAtIndex(rs);
    if (address % 2 != 0) {
        cop0->badVirtualAddress = address;
        triggerException(ExceptionType::LoadAddress);
        return;
    }
    jumpDestination = address;
    isBranching = true;
}

void CPU::operationJumpAndLinkRegister(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs);
    uint32_t returnAddress = programCounter + 8;

    setRegisterAtIndex(rd, returnAddress);
    if (address % 2 != 0) {
        cop0->badVirtualAddress = address;
        triggerException(ExceptionType::LoadAddress);
        return;
    }
    jumpDestination = address;
    isBranching = true;
}

void CPU::operationSystemCall(Instruction instruction) {
    TestRunner *testRunner = TestRunner::getInstance();
    if (testRunner->shouldLogBiosFunctionCalls()) {
        printWarning("  SYSCALL: %#x", registers[4]);
    }
    triggerException(ExceptionType::SysCall);
}

void CPU::operationBreak(Instruction instruction) {
    triggerException(ExceptionType::Breakpoint);
}

void CPU::operationMoveFromHighRegister(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t high = highRegister;

    setRegisterAtIndex(rd, high);
}

void CPU::operationMoveToHighRegister(Instruction instruction) {
    uint32_t rs = instruction.rs;

    highRegister = registerAtIndex(rs);
}

void CPU::operationMoveFromLowRegister(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t low = lowRegister;

    setRegisterAtIndex(rd, low);
}

void CPU::operationMoveToLowRegister(Instruction instruction) {
    uint32_t rs = instruction.rs;

    lowRegister = registerAtIndex(rs);
}

void CPU::operationMultiply(Instruction instruction) {
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    int64_t s = ((int32_t)registerAtIndex(rs));
    int64_t t = ((int32_t)registerAtIndex(rt));

    uint64_t result = s * t;
    highRegister = ((uint32_t)(result >> 32));
    lowRegister = ((uint32_t)result & 0xFFFFFFFF);
}

void CPU::operationMultiplyUnsigned(Instruction instruction) {
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint64_t s = registerAtIndex(rs);
    uint64_t t = registerAtIndex(rt);

    uint64_t result = s * t;
    highRegister = ((uint32_t)(result >> 32));
    lowRegister = ((uint32_t)result & 0xFFFFFFFF);
}

void CPU::operationDivision(Instruction instruction) {
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    int32_t n = registerAtIndex(rs);
    int32_t d = registerAtIndex(rt);

    if (d == 0) {
        highRegister = (uint32_t)n;
        if (n >= 0) {
            lowRegister = 0xffffffff;
        } else {
            lowRegister = 0x1;
        }
    } else if (((uint32_t)n) == 0x80000000 && ((uint32_t)d) == 0xffffffff) {
        highRegister = 0;
        lowRegister = 0x80000000;
    } else {
        highRegister = ((uint32_t)(n % d));
        lowRegister = ((uint32_t)(n / d));
    }
}

void CPU::operationDivisionUnsigned(Instruction instruction) {
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint32_t n = registerAtIndex(rs);
    uint32_t d = registerAtIndex(rt);

    if (d == 0) {
        highRegister = n;
        lowRegister = 0xffffffff;
    } else {
        highRegister = n % d;
        lowRegister = n / d;
    }
}

void CPU::operationAdd(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint32_t s = registerAtIndex(rs);
    uint32_t t = registerAtIndex(rt);

    uint32_t result = s + t;
    if (!((s ^ t) & 0x80000000) && ((result ^ s) & 0x80000000)) {
        triggerException(ExceptionType::Overflow);
        return;
    }

    setRegisterAtIndex(rd, result);
}

void CPU::operationAddUnsigned(Instruction instruction) {
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;
    uint32_t rd = instruction.rd;

    uint32_t value = registerAtIndex(rs) + registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}

void CPU::operationSubstract(Instruction instruction) {
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;
    uint32_t rd = instruction.rd;

    uint32_t s = registerAtIndex(rs);
    uint32_t t = registerAtIndex(rt);
    uint32_t result = s - t;
    if (((s ^ t) & 0x80000000) && ((result ^ s) & 0x80000000)) {
        triggerException(ExceptionType::Overflow);
        return;
    } else {
        setRegisterAtIndex(rd, result);
    }
}

void CPU::operationSubstractUnsigned(Instruction instruction) {
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;
    uint32_t rd = instruction.rd;

    uint32_t value = registerAtIndex(rs) - registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}

void CPU::operationBitwiseAnd(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint32_t value = registerAtIndex(rs) & registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}

void CPU::operationBitwiseOr(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint32_t value = registerAtIndex(rs) | registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}

void CPU::operationBitwiseExclusiveOr(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint32_t value = registerAtIndex(rs) ^ registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}

void CPU::operationBitwiseNotOr(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint32_t value = ~(registerAtIndex(rs) | registerAtIndex(rt));
    setRegisterAtIndex(rd, value);
}

void CPU::operationSetOnLessThan(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    int32_t s = (int32_t)registerAtIndex(rs);
    int32_t t = (int32_t)registerAtIndex(rt);
    uint32_t value = s < t;
    setRegisterAtIndex(rd, value);
}

void CPU::operationSetOnLessThanUnsigned(Instruction instruction) {
    uint32_t rd = instruction.rd;
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint32_t value = registerAtIndex(rs) < registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}

void CPU::operationIllegal(Instruction instruction) {
    triggerException(ExceptionType::Illegal);
}

// Multipe branch-if instructions
// 000001 | rs   | 00000| <--immediate16bit--> | bltz
// 000001 | rs   | 00001| <--immediate16bit--> | bgez
// 000001 | rs   | 10000| <--immediate16bit--> | bltzal
// 000001 | rs   | 10001| <--immediate16bit--> | bgezal
void CPU::operationsMultipleBranchIf(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rs = instruction.rs;

    bool isGreatherThanOrEqualToZero = instruction.rt & 0x01;
    bool shouldLink = (instruction.rt & 0x1e) == 0x10;

    int32_t value = registerAtIndex(rs);
    bool result;
    if (isGreatherThanOrEqualToZero) {
        result = value >= 0;
    } else {
        result = value < 0;
    }

    if (shouldLink) {
        registers[31] = programCounter + 8;
    }

    if (result) {
        branch(imm);
    }
}

void CPU::operationJump(Instruction instruction) {
    uint32_t imm = instruction.immjump();
    jumpDestination = (programCounter & 0xF0000000) | (imm << 2);
    isBranching = true;
}

void CPU::operationJumpAndLink(Instruction instruction) {
    uint32_t returnAddress = programCounter + 8;
    operationJump(instruction);
    registers[31] = returnAddress;
}

void CPU::operationBranchIfEqual(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    if (registerAtIndex(rs) == registerAtIndex(rt)) {
        branch(imm);
    }
}

void CPU::operationBranchIfNotEqual(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    if (registerAtIndex(rs) != registerAtIndex(rt)) {
        branch(imm);
    }
}

void CPU::operationBranchIfLessThanOrEqualToZero(Instruction instruction) {
    uint32_t imm =  instruction.immSE();
    uint32_t rs = instruction.rs;

    int32_t value = registerAtIndex(rs);
    if (value <= 0) {
        branch(imm);
    }
}

void CPU::operationBranchIfGreaterThanZero(Instruction instruction) {
    uint32_t imm =  instruction.immSE();
    uint32_t rs = instruction.rs;

    int32_t value = registerAtIndex(rs);
    if (value > 0) {
        branch(imm);
    }
}

void CPU::operationAddImmediate(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t value = registerAtIndex(rs);
    uint32_t result = value + imm;
    if (!((value ^ imm) & 0x80000000) && ((result ^ value) & 0x80000000)) {
        triggerException(ExceptionType::Overflow);
    } else {
        setRegisterAtIndex(rt, result);
    }
}

void CPU::operationAddImmediateUnsigned(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t value = registerAtIndex(rs) + imm;
    setRegisterAtIndex(rt, value);
}

void CPU::operationSetIfLessThanImmediate(Instruction instruction) {
    int32_t imm = instruction.immSE();
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint32_t value = ((int32_t)registerAtIndex(rs)) < imm;
    setRegisterAtIndex(rt, value);
}

void CPU::operationSetIfLessThanImmediateUnsigned(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rs = instruction.rs;
    uint32_t rt = instruction.rt;

    uint32_t value = registerAtIndex(rs) < imm;
    setRegisterAtIndex(rt, value);
}

void CPU::operationBitwiseAndImmediate(Instruction instruction) {
    uint32_t imm = instruction.imm();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t value = registerAtIndex(rs) & imm;

    setRegisterAtIndex(rt, value);
}

void CPU::operationBitwiseOrImmediate(Instruction instruction) {
    uint32_t imm = instruction.imm();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t value = registerAtIndex(rs) | imm;

    setRegisterAtIndex(rt, value);
}

void CPU::operationBitwiseExclusiveOrImmediate(Instruction instruction) {
    uint32_t imm = instruction.imm();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t value = registerAtIndex(rs) ^ imm;
    setRegisterAtIndex(rt, value);
}

void CPU::operationLoadUpperImmediate(Instruction instruction) {
    uint32_t imm = instruction.imm();
    uint32_t rt = instruction.rt;

    uint32_t value = imm << 16;
    setRegisterAtIndex(rt, value);
}

void CPU::branch(uint32_t offset) {
    // Align to 32 bits
    offset <<= 2;
    jumpDestination = ((int32_t)(programCounter + 4) + offset);
    isBranching = true;
}

void CPU::operationCoprocessor0(Instruction instruction) {
    switch (instruction.copcode()) {
        case 0b00000: {
            operationMoveFromCoprocessor0(instruction);
            break;
        }
        case 0b00100: {
            operationMoveToCoprocessor0(instruction);
            break;
        }
        case 0b10000: {
            operationReturnFromException(instruction);
            break;
        }
        default: {
            printError("Unhandled coprocessor0 instruction %#x", instruction.value);
        }
    }
}

void CPU::operationMoveFromCoprocessor0(Instruction instruction) {
    uint32_t cpuRegisterIndex = instruction.rt;
    uint32_t copRegisterIndex = instruction.rd;

    uint32_t value;
    switch (copRegisterIndex) {
        case 3: {
            value = cop0->breakPointOnExecute;
            break;
        }
        case 5: {
            value = cop0->breakPointOnDataAccess;
            break;
        }
        case 6: {
            value = cop0->jumpDestination;
            break;
        }
        case 7: {
            value = cop0->breakPointControl;
            break;
        }
        case 8: {
            value = cop0->badVirtualAddress;
            break;
        }
        case 9: {
            value = cop0->dataAccessBreakpointMask;
            break;
        }
        case 11: {
            value = cop0->executeBreakpointMask;
            break;
        }
        case 12: {
            value = cop0->status.value;
            break;
        }
        case 13: {
            value = cop0->cause.value;
            break;
        }
        case 14: {
            value = cop0->returnAddressFromTrap;
            break;
        }
        case 15: {
            value = cop0->processorID;
            break;
        }
        default: {
            printError("Unhandled MFC0 at index %#x", copRegisterIndex);
        }
    }
    loadDelaySlot(cpuRegisterIndex, value);
}

void CPU::operationMoveToCoprocessor0(Instruction instruction) {
    uint32_t cpuRegisterIndex = instruction.rt;
    uint32_t copRegisterIndex = instruction.rd;

    uint32_t value = registerAtIndex(cpuRegisterIndex);

    switch (copRegisterIndex) {
        case 3: {
            cop0->breakPointOnExecute = value;
            break;
        }
        case 5: {
            cop0->breakPointOnDataAccess = value;
            break;
        }
        case 6: {
            // No idea why this happens, this isn't R/W accoring to no$. According to EmuDev Discord:
            // "JUMPDEST is somewhat of a name no$ just gave, the PS devkit describes that as PIDMASK"
            cop0->jumpDestination = value;
            break;
        }
        case 7: {
            cop0->breakPointControl = value;
            break;
        }
        case 9: {
            cop0->dataAccessBreakpointMask = value;
            break;
        }
        case 11: {
            cop0->executeBreakpointMask = value;
            break;
        }
        case 12: {
            cop0->status.value = value;
            break;
        }
        case 13: {
            // Only bit 8 and 9 can be written. See COP0.hpp
            cop0->cause.value &= ~0x300;
            cop0->cause.value |= (value & 0x300);
            break;
        }
        default: {
            printError("Unhandled MTC0 at index %d", copRegisterIndex);
        }
    }
}

void CPU::operationReturnFromException(Instruction instruction) {
    if (instruction.subfunct != 0b010000) {
        printError("Unhandled cop0 instruction (0b010000) with last 6 bits: %#x", instruction.subfunct);
    }
    cop0->status.currentInterruptEnable = cop0->status.previousInterruptEnable;
    cop0->status._currentOperationMode = cop0->status._previousOperationMode;

    cop0->status.previousInterruptEnable = cop0->status.oldInterruptEnable;
    cop0->status._previousOperationMode = cop0->status._oldOperationMode;
}

void CPU::operationCoprocessor1(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}

void CPU::operationCoprocessor2(Instruction instruction) {
    printError("Unhandled Geometry Translation Engine instruction: %#x", instruction.value);
}

void CPU::operationCoprocessor3(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}

void CPU::operationLoadByte(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs) + imm;
    uint32_t value = ((int32_t)(load<uint8_t>(address) << 24)) >> 24;
    loadDelaySlot(rt, value);
}

void CPU::operationLoadHalfWord(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs) + imm;
    if (address % 2 != 0) {
        cop0->badVirtualAddress = address;
        triggerException(ExceptionType::LoadAddress);
        return;
    }
    uint32_t value = ((int16_t)load<uint16_t>(address));
    loadDelaySlot(rt, value);
}

void CPU::operationLoadWordLeft(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs) + imm;
    uint32_t currentValue;
    if (loadSlots[0].registerIndex == rt) {
        currentValue = loadSlots[0].value;
    } else {
        currentValue = registerAtIndex(rt);
    }

    uint32_t alignedAddress = address & 0xfffffffc;
    uint32_t alignedWord = load<uint32_t>(alignedAddress);

    uint32_t value;
    switch (address & 3) {
        case 0: {
            value = (currentValue & 0x00ffffff) | (alignedWord << 24);
            break;
        }
        case 1: {
            value = (currentValue & 0x0000ffff) | (alignedWord << 16);
            break;
        }
        case 2: {
            value = (currentValue & 0x000000ff) | (alignedWord << 8);
            break;
        }
        case 3: {
            value = (currentValue & 0x00000000) | (alignedWord << 0);
            break;
        }
    }

    loadDelaySlot(rt, value);
}

void CPU::operationLoadWord(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs) + imm;
    if (address % 4 != 0) {
        cop0->badVirtualAddress = address;
        triggerException(ExceptionType::LoadAddress);
        return;
    }
    uint32_t value = load<uint32_t>(address);
    loadDelaySlot(rt, value);
}

void CPU::operationLoadByteUnsigned(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs) + imm;
    uint32_t value = load<uint8_t>(address);
    loadDelaySlot(rt, value);
}

void CPU::operationLoadHalfWordUnsigned(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs) + imm;
    if (address % 2 != 0) {
        cop0->badVirtualAddress = address;
        triggerException(ExceptionType::LoadAddress);
        return;
    }
    uint32_t value = load<uint16_t>(address);
    loadDelaySlot(rt, value);
}

void CPU::operationLoadWordRight(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs) + imm;
    uint32_t currentValue;
    if (loadSlots[0].registerIndex == rt) {
        currentValue = loadSlots[0].value;
    } else {
        currentValue = registerAtIndex(rt);
    }

    uint32_t alignedAddress = address & 0xfffffffc;
    uint32_t alignedWord = load<uint32_t>(alignedAddress);

    uint32_t value;
    switch (address & 3) {
        case 0: {
            value = (currentValue & 0x00000000) | (alignedWord >> 0);
            break;
        }
        case 1: {
            value = (currentValue & 0xff000000) | (alignedWord >> 8);
            break;
        }
        case 2: {
            value = (currentValue & 0xffff0000) | (alignedWord >> 16);
            break;
        }
        case 3: {
            value = (currentValue & 0xffffff00) | (alignedWord >> 24);
            break;
        }
    }

    loadDelaySlot(rt, value);
}

void CPU::operationStoreByte(Instruction instruction) const {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs) + imm;

    uint32_t value = registerAtIndex(rt);
    store<uint8_t>(address, value);
}

void CPU::operationStoreHalfWord(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs) + imm;
    if (address % 2 != 0) {
        cop0->badVirtualAddress = address;
        triggerException(ExceptionType::StoreAddress);
        return;
    }

    uint32_t value = registerAtIndex(rt);
    store<uint16_t>(address, value);
}

void CPU::operationStoreWordLeft(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs) + imm;
    uint32_t value = registerAtIndex(rt);

    uint32_t alignedAddress = address & 0xfffffffc;
    uint32_t currentMemoryValue = load<uint32_t>(alignedAddress);

    uint32_t memoryValue;
    switch (address & 3) {
        case 0: {
            memoryValue = (currentMemoryValue & 0xffffff00) | (value >> 24);
            break;
        }
        case 1: {
            memoryValue = (currentMemoryValue & 0xffff0000) | (value >> 16);
            break;
        }
        case 2: {
            memoryValue = (currentMemoryValue & 0xff000000) | (value >> 8);
            break;
        }
        case 3: {
            memoryValue = (currentMemoryValue & 0x00000000) | (value >> 0);
            break;
        }
    }

    store<uint32_t>(alignedAddress, memoryValue);
}

void CPU::operationStoreWord(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs) + imm;
    if (address % 4 != 0) {
        cop0->badVirtualAddress = address;
        triggerException(ExceptionType::StoreAddress);
        return;
    }

    uint32_t value = registerAtIndex(rt);
    store<uint32_t>(address, value);
}

void CPU::operationStoreWordRight(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt;
    uint32_t rs = instruction.rs;

    uint32_t address = registerAtIndex(rs) + imm;
    uint32_t value = registerAtIndex(rt);

    uint32_t alignedAddress = address & 0xfffffffc;
    uint32_t currentMemoryValue = load<uint32_t>(alignedAddress);

    uint32_t memoryValue;
    switch (address & 3) {
        case 0: {
            memoryValue = (currentMemoryValue & 0x00000000) | (value << 0);
            break;
        }
        case 1: {
            memoryValue = (currentMemoryValue & 0x000000ff) | (value << 8);
            break;
        }
        case 2: {
            memoryValue = (currentMemoryValue & 0x0000ffff) | (value << 16);
            break;
        }
        case 3: {
            memoryValue = (currentMemoryValue & 0x00ffffff) | (value << 24);
            break;
        }
    }

    store<uint32_t>(alignedAddress, memoryValue);
}

void CPU::triggerException(ExceptionType exceptionType) {
    cop0->cause._exception = exceptionType;

    if (exceptionType != BusErrorOnInstruction) {
        cop0->cause.coprocessorNumber = currentInstruction.funct & 0x3;
    }

    cop0->status.oldInterruptEnable = cop0->status.previousInterruptEnable;
    cop0->status._oldOperationMode = cop0->status._previousOperationMode;

    cop0->status.previousInterruptEnable = cop0->status.currentInterruptEnable;
    cop0->status._previousOperationMode = cop0->status._currentOperationMode;

    cop0->status.currentInterruptEnable = false;
    cop0->status._currentOperationMode = Kernel;

    if (isBranching) {
        cop0->returnAddressFromTrap = programCounter - 4;
        cop0->cause.branchDelay = true;
    } else {
        cop0->returnAddressFromTrap = programCounter;
        cop0->cause.branchDelay = false;
    }

    uint32_t handlerAddress;
    if (cop0->status.bootExceptionVectors() == BootExceptionVectors::ROMV) {
        handlerAddress = 0xbfc00180;
    } else {
        handlerAddress = 0x80000080;
    }

    programCounter = handlerAddress;
    isBranching = false;
    runningException = true;
}

void CPU::operationLoadWordCoprocessor0(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}

void CPU::operationLoadWordCoprocessor1(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}

void CPU::operationLoadWordCoprocessor2(Instruction instruction) {
    printError("Unhandled GTE LWC: %#x", instruction.value);
}

void CPU::operationLoadWordCoprocessor3(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}

void CPU::operationStoreWordCoprocessor0(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}

void CPU::operationStoreWordCoprocessor1(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}

void CPU::operationStoreWordCoprocessor2(Instruction instruction) {
    printError("Unhandled GTE SWC: %#x", instruction.value);
}

void CPU::operationStoreWordCoprocessor3(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}
