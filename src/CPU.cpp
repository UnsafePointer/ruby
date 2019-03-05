#include "CPU.hpp"
#include <algorithm>
#include <iostream>
#include <tuple>

using namespace std;

CPU::CPU() : programCounter(0xbfc00000),
             currentProgramCounter(0xbfc00000),
             isBranching(false),
             isDelaySlot(false),
             load({RegisterIndex(), 0}),
             statusRegister(0),
             highRegister(0xdeadbeef),
             lowRegister(0xdeadbeef)
{
    interconnect = make_unique<Interconnect>();
    nextProgramCounter = programCounter + 4;
    fill_n(registers, 32, 0xDEADBEEF);
    registers[0] = 0;
    copy(begin(registers), end(registers), begin(outputRegisters));
}

CPU::~CPU() {
}

uint32_t CPU::loadWord(uint32_t address) const {
    return interconnect->loadWord(address);
}

uint16_t CPU::loadHalfWord(uint32_t address) const {
    return interconnect->loadHalfWord(address);
}

uint8_t CPU::loadByte(uint32_t address) const {
    return interconnect->loadByte(address);
}

void CPU::storeWord(uint32_t address, uint32_t value) const {
    return interconnect->storeWord(address, value);
}

void CPU::storeHalfWord(uint32_t address, uint16_t value) const {
    return interconnect->storeHalfWord(address, value);
}

void CPU::storeByte(uint32_t address, uint8_t value) const {
    return interconnect->storeByte(address, value);
}

void CPU::executeNextInstruction() {
    currentProgramCounter = programCounter;
    if (currentProgramCounter % 4 != 0) {
        triggerException(ExceptionType::LoadAddress);
        return;
    }
    Instruction instruction = Instruction(loadWord(programCounter));

    isDelaySlot = isBranching;
    isBranching = false;

    programCounter = nextProgramCounter;
    nextProgramCounter = nextProgramCounter + 4;

    RegisterIndex loadRegisterIndex;
    uint32_t value;
    tie(loadRegisterIndex, value) = load;
    setRegisterAtIndex(loadRegisterIndex, value);
    load = {RegisterIndex(), 0};

    decodeAndExecuteInstruction(instruction);
    copy(begin(outputRegisters), end(outputRegisters), begin(registers));
}

uint32_t CPU::registerAtIndex(RegisterIndex index) const {
    return registers[index.idx()];
}

void CPU::setRegisterAtIndex(RegisterIndex index, uint32_t value) {
    outputRegisters[index.idx()] = value;

    // Make sure R0 is always 0
    outputRegisters[0] = 0;
}

void CPU::decodeAndExecuteInstruction(Instruction instruction) {
    switch (instruction.funct()) {
        case 0b000000: {
            switch (instruction.subfunct()) {
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

void CPU::branch(uint32_t offset) {
    // Align to 32 bits
    offset <<= 2;
    nextProgramCounter = programCounter + offset;
    isBranching = true;
}

void CPU::operationCoprocessor0(Instruction instruction) {
    switch (instruction.copcode()) {
        case 0b00100: {
            operationMoveToCoprocessor0(instruction);
            break;
        }
        case 0b00000: {
            operationMoveFromCoprocessor0(instruction);
            break;
        }
        case 0b10000: {
            operationReturnFromException(instruction);
            break;
        }
        default: {
            cout << "Unhandled coprocessor0 instruction 0x" << hex << instruction.dat() << endl;
            exit(1);
        }
    }
}


void CPU::operationMoveToCoprocessor0(Instruction instruction) {
    RegisterIndex cpuRegisterIndex = instruction.rt();
    RegisterIndex copRegisterIndex = instruction.rd();

    uint32_t value = registerAtIndex(cpuRegisterIndex);

    switch (copRegisterIndex.idx()) {
        case 3:
        case 5:
        case 6:
        case 7:
        case 9:
        case 11: {
            if (value != 0) {
                cout << "Unhandled MTC0 at index " << copRegisterIndex.idx() << endl;
                exit(1);
            }
            break;
        }
        case 12: {
            statusRegister = value;
            break;
        }
        case 13: {
            causeRegister = value;
            break;
        }
        case 14: {
            returnAddressFromTrap = value;
            break;
        }
        default: {
            cout << "Unhandled MTC0 at index " << copRegisterIndex.idx() << endl;
            exit(1);
        }
    }
}

void CPU::operationLoadUpperImmediate(Instruction instruction) {
    uint32_t imm = instruction.imm();
    RegisterIndex rt = instruction.rt();

    uint32_t value = imm << 16;
    setRegisterAtIndex(rt, value);
}

void CPU::operationBitwiseOrImmediate(Instruction instruction) {
    uint32_t imm = instruction.imm();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t value = registerAtIndex(rs) | imm;

    setRegisterAtIndex(rt, value);
}

void CPU::operationStoreWord(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    if ((statusRegister & 0x10000) != 0) {
        cout << "Cache is isolated, ignoring store at address: 0x" << hex << address << endl;
        return;
    }
    if (address % 4 != 0) {
        triggerException(ExceptionType::StoreAddress);
        return;
    }

    uint32_t value = registerAtIndex(rt);
    storeWord(address, value);
}

void CPU::operationShiftLeftLogical(Instruction instruction) {
    uint32_t imm = instruction.shiftimm();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rd = instruction.rd();

    uint32_t value = registerAtIndex(rt) << imm;
    setRegisterAtIndex(rd, value);
}

void CPU::operationAddImmediateUnsigned(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t value = registerAtIndex(rs) + imm;
    setRegisterAtIndex(rt, value);
}

void CPU::operationJump(Instruction instruction) {
    uint32_t imm = instruction.immjump();
    nextProgramCounter = (programCounter & 0xF0000000) | (imm << 2);
    isBranching = true;
}

void CPU::operationBitwiseOr(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    uint32_t value = registerAtIndex(rs) | registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}

void CPU::operationBranchIfNotEqual(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    if (registerAtIndex(rs) != registerAtIndex(rt)) {
        branch(imm);
    }
}

void CPU::operationAddImmediate(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t value = registerAtIndex(rs);
    uint32_t result = value + imm;
    if (!((value ^ imm) & 0x80000000) && ((result ^ value) & 0x80000000)) {
        triggerException(ExceptionType::Overflow);
    } else {
        setRegisterAtIndex(rt, result);
    }
}

void CPU::operationLoadWord(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    if ((statusRegister & 0x10000) != 0) {
        cout << "Cache is isolated, ignoring store at address: 0x" << hex << address << endl;
        return;
    }
    if (address % 4 != 0) {
        triggerException(ExceptionType::LoadAddress);
        return;
    }
    uint32_t value = loadWord(address);
    load = {rt, value};
}

void CPU::operationSetOnLessThanUnsigned(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    uint32_t value = registerAtIndex(rs) < registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}

void CPU::operationAddUnsigned(Instruction instruction) {
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rd = instruction.rd();

    uint32_t value = registerAtIndex(rs) + registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}

void CPU::operationStoreHalfWord(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    if ((statusRegister & 0x10000) != 0) {
        cout << "Cache is isolated, ignoring store at address: 0x" << hex << address << endl;
        return;
    }
    if (address % 2 != 0) {
        triggerException(ExceptionType::StoreAddress);
        return;
    }

    uint32_t value = registerAtIndex(rt);
    storeHalfWord(address, value);
}

void CPU::operationJumpAndLink(Instruction instruction) {
    uint32_t returnAddress = nextProgramCounter;
    operationJump(instruction);
    setRegisterAtIndex(RegisterIndex(31), returnAddress);
}

void CPU::operationBitwiseAndImmediate(Instruction instruction) {
    uint32_t imm = instruction.imm();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t value = registerAtIndex(rs) & imm;

    setRegisterAtIndex(rt, value);
}

void CPU::operationStoreByte(Instruction instruction) const {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    if ((statusRegister & 0x10000) != 0) {
        cout << "Cache is isolated, ignoring store at address: 0x" << hex << address << endl;
        return;
    }

    uint32_t value = registerAtIndex(rt);
    storeByte(address, value);
}

void CPU::operationJumpRegister(Instruction instruction) {
    RegisterIndex rs = instruction.rs();
    nextProgramCounter = registerAtIndex(rs);
    isBranching = true;
}

void CPU::operationLoadByte(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    if ((statusRegister & 0x10000) != 0) {
        cout << "Cache is isolated, ignoring store at address: 0x" << hex << address << endl;
        return;
    }
    uint32_t value = (int8_t)loadByte(address);
    load = {rt, value};
}

void CPU::operationBranchIfEqual(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    if (registerAtIndex(rs) == registerAtIndex(rt)) {
        branch(imm);
    }
}

void CPU::operationMoveFromCoprocessor0(Instruction instruction) {
    RegisterIndex cpuRegisterIndex = instruction.rt();
    RegisterIndex copRegisterIndex = instruction.rd();

    uint32_t value;
    switch (copRegisterIndex.idx()) {
        case 12: {
            value = statusRegister;
            break;
        }
        case 13: {
            value = causeRegister;
            break;
        }
        case 14: {
            value = returnAddressFromTrap;
            break;
        }
        default: {
            cout << "Unhandled MFC0 at index " << copRegisterIndex.idx() << endl;
            exit(1);
        }
    }
    load = {cpuRegisterIndex, value};
}

void CPU::operationBitwiseAnd(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    uint32_t value = registerAtIndex(rs) & registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}

void CPU::operationAdd(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    int64_t s = (int32_t)registerAtIndex(rs);
    int64_t t = (int32_t)registerAtIndex(rt);

    int64_t result = s + t;
    if (result < INT32_MIN || result > INT32_MAX) {
        triggerException(ExceptionType::Overflow);
    }

    setRegisterAtIndex(rd, result);
}

void CPU::operationBranchIfGreaterThanZero(Instruction instruction) {
    uint32_t imm =  instruction.immSE();
    RegisterIndex rs = instruction.rs();

    int32_t value = registerAtIndex(rs);
    if (value > 0) {
        branch(imm);
    }
}

void CPU::operationBranchIfLessThanOrEqualToZero(Instruction instruction) {
    uint32_t imm =  instruction.immSE();
    RegisterIndex rs = instruction.rs();

    int32_t value = registerAtIndex(rs);
    if (value <= 0) {
        branch(imm);
    }
}

void CPU::operationLoadByteUnsigned(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    if ((statusRegister & 0x10000) != 0) {
        cout << "Cache is isolated, ignoring store at address: 0x" << hex << address << endl;
        return;
    }
    uint32_t value = loadByte(address);
    load = {rt, value};
}


void CPU::operationJumpAndLinkRegister(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    RegisterIndex rs = instruction.rs();

    uint32_t returnAddress = nextProgramCounter;

    setRegisterAtIndex(rd, returnAddress);
    nextProgramCounter = registerAtIndex(rs);
    isBranching = true;
}

// Multipe branch-if instructions
// 000001 | rs   | 00000| <--immediate16bit--> | bltz
// 000001 | rs   | 00001| <--immediate16bit--> | bgez
// 000001 | rs   | 10000| <--immediate16bit--> | bltzal
// 000001 | rs   | 10001| <--immediate16bit--> | bgezal
void CPU::operationsMultipleBranchIf(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rs = instruction.rs();

    uint32_t data = instruction.dat();
    bool isGreatherThanOrEqualToZero = (data >> 16) & 1;
    bool shouldLink = ((data >> 17) & 0xF) == 0x8;

    int32_t value = registerAtIndex(rs);
    bool result;
    if (isGreatherThanOrEqualToZero) {
        result = value >= 0;
    } else {
        result = value < 0;
    }

    if (shouldLink) {
        uint32_t returnAddress = programCounter;
        setRegisterAtIndex(RegisterIndex(31), returnAddress);
    }

    if (result) {
        branch(imm);
    }
}

void CPU::operationSetIfLessThanImmediate(Instruction instruction) {
    int32_t imm = instruction.immSE();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    uint32_t value = ((int32_t)registerAtIndex(rs)) < imm;
    setRegisterAtIndex(rt, value);
}

void CPU::operationSubstractUnsigned(Instruction instruction) {
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rd = instruction.rd();

    uint32_t value = registerAtIndex(rs) - registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}

void CPU::operationShiftRightArithmetic(Instruction instruction) {
    uint32_t imm = instruction.shiftimm();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rd = instruction.rd();

    uint32_t value = ((int32_t)registerAtIndex(rt)) >> imm;
    setRegisterAtIndex(rd, value);
}

void CPU::operationDivision(Instruction instruction) {
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    int32_t n = registerAtIndex(rs);
    int32_t d = registerAtIndex(rt);

    if (d == 0) {
        highRegister = (uint32_t)n;
        if (n >= 0) {
            lowRegister = 0xffffffff;
        } else {
            lowRegister = 0x1;
        }
    } else if (((uint32_t)n) == 0x80000000 && d == -1) {
        highRegister = 0;
        lowRegister = 0x80000000;
    } else {
        highRegister = ((uint32_t)(n % d));
        lowRegister = ((uint32_t)(n / d));
    }
}

void CPU::operationMoveFromLowRegister(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    uint32_t low = lowRegister;

    setRegisterAtIndex(rd, low);
}

void CPU::operationShiftRightLogical(Instruction instruction) {
    uint32_t imm = instruction.shiftimm();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rd = instruction.rd();

    uint32_t value = registerAtIndex(rt) >> imm;
    setRegisterAtIndex(rd, value);
}

void CPU::operationSetIfLessThanImmediateUnsigned(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    uint32_t value = registerAtIndex(rs) < imm;
    setRegisterAtIndex(rt, value);
}

void CPU::operationDivisionUnsigned(Instruction instruction) {
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

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

void CPU::operationMoveFromHighRegister(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    uint32_t high = highRegister;

    setRegisterAtIndex(rd, high);
}

void CPU::operationSetOnLessThan(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    int32_t s = (int32_t)registerAtIndex(rs);
    int32_t t = (int32_t)registerAtIndex(rt);
    uint32_t value = s < t;
    setRegisterAtIndex(rd, value);
}

void CPU::triggerException(ExceptionType exceptionType) {
    uint32_t handlerAddress = 0x80000000;
    if ((statusRegister & (1 << 2)) != 0) {
        handlerAddress = 0xbfc00180;
    }

    // Grab the last 6 bits of SR
    // Shift the result two spaces to the left, padded with zeroes
    // This is the Interrupt Enable/User Mode mask
    uint32_t mode = statusRegister & 0x3f;
    statusRegister &= ~0x3f;
    statusRegister |= ((mode << 2) & 0x3f);

    causeRegister = exceptionType << 2;

    returnAddressFromTrap = currentProgramCounter;
    if (isDelaySlot) {
        returnAddressFromTrap = returnAddressFromTrap - 4;
        causeRegister |= 1 << 31;
    }

    programCounter = handlerAddress;
    nextProgramCounter = programCounter + 4;
}

void CPU::operationSystemCall(Instruction instruction) {
    triggerException(ExceptionType::SysCall);
}

void CPU::operationMoveToLowRegister(Instruction instruction) {
    RegisterIndex rs = instruction.rs();

    lowRegister = registerAtIndex(rs);
}

void CPU::operationMoveToHighRegister(Instruction instruction) {
    RegisterIndex rs = instruction.rs();

    highRegister = registerAtIndex(rs);
}

void CPU::operationReturnFromException(Instruction instruction) {
    if (instruction.subfunct() != 0b010000) {
        cout << "Unhandled cop0 instruction (0b010000) with last 6 bits: 0x" << hex << instruction.subfunct() << endl;
        exit(1);
    }

    uint32_t mode = statusRegister & 0x3f;
    statusRegister &= !0x3f;
    statusRegister |= mode >> 2;
}

void CPU::operationLoadHalfWordUnsigned(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    if ((statusRegister & 0x10000) != 0) {
        cout << "Cache is isolated, ignoring load at address: 0x" << hex << address << endl;
        return;
    }
    if (address % 2 != 0) {
        triggerException(ExceptionType::LoadAddress);
        return;
    }
    uint32_t value = loadHalfWord(address);
    load = {rt, value};
}

void CPU::operationShiftLeftLogicalVariable(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    uint32_t value = registerAtIndex(rt) << (registerAtIndex(rs) & 0x1f);
    setRegisterAtIndex(rd, value);
}

void CPU::operationLoadHalfWord(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    if ((statusRegister & 0x10000) != 0) {
        cout << "Cache is isolated, ignoring load at address: 0x" << hex << address << endl;
        return;
    }
    if (address % 2 != 0) {
        triggerException(ExceptionType::LoadAddress);
        return;
    }
    uint32_t value = ((int16_t)loadHalfWord(address));
    load = {rt, value};
}

void CPU::operationBitwiseNotOr(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    uint32_t value = !(registerAtIndex(rs) | registerAtIndex(rt));
    setRegisterAtIndex(rd, value);
}

void CPU::operationShiftRightArithmeticVariable(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    uint32_t value = ((int32_t)registerAtIndex(rt)) >> (registerAtIndex(rs) & 0x1f);
    setRegisterAtIndex(rd, value);
}

void CPU::operationShiftRightLogicalVariable(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    uint32_t value = registerAtIndex(rt) >> (registerAtIndex(rs) & 0x1f);
    setRegisterAtIndex(rd, value);
}

void CPU::operationMultiplyUnsigned(Instruction instruction) {
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    uint64_t s = registerAtIndex(rs);
    uint64_t t = registerAtIndex(rt);

    uint64_t result = s * t;
    highRegister = ((uint32_t)(result >> 32));
    lowRegister = ((uint32_t)result);
}

void CPU::operationBitwiseExclusiveOr(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    uint32_t value = registerAtIndex(rs) ^ registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}

void CPU::operationBreak(Instruction instruction) {
    triggerException(ExceptionType::Break);
}

void CPU::operationMultiply(Instruction instruction) {
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    int64_t s = ((int64_t)registerAtIndex(rs));
    int64_t t = ((int64_t)registerAtIndex(rt));

    uint64_t result = s * t;
    highRegister = ((uint32_t)(result >> 32));
    lowRegister = ((uint32_t)result);
}

void CPU::operationSubstract(Instruction instruction) {
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rd = instruction.rd();

    int32_t s = registerAtIndex(rs);
    int32_t t = registerAtIndex(rt);
    uint32_t result = s - t;
    if (((s ^ t) & 0x80000000) && ((result ^ s) & 0x80000000)) {
        triggerException(ExceptionType::Overflow);
        return;
    } else {
        setRegisterAtIndex(rd, result);
    }
}

void CPU::operationBitwiseExclusiveOrImmediate(Instruction instruction) {
    uint32_t imm = instruction.imm();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t value = registerAtIndex(rs) ^ imm;
    setRegisterAtIndex(rt, value);
}

void CPU::operationCoprocessor1(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}

void CPU::operationCoprocessor2(Instruction instruction) {
    cout << "Unhandled Geometry Translation Engine instruction: 0x" << hex << instruction.dat() << endl;
    exit(1);
}

void CPU::operationCoprocessor3(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}

void CPU::operationLoadWordLeft(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    uint32_t currentValue = outputRegisters[rt.idx()];

    uint32_t alignedAddress = address & !3;
    uint32_t alignedWord = loadWord(alignedAddress);

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

    load = {rt, value};
}

void CPU::operationLoadWordRight(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    uint32_t currentValue = outputRegisters[rt.idx()];

    uint32_t alignedAddress = address & !3;
    uint32_t alignedWord = loadWord(alignedAddress);

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

    load = {rt, value};
}

void CPU::operationStoreWordLeft(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    uint32_t value = registerAtIndex(rt);

    uint32_t alignedAddress = address & !3;
    uint32_t currentMemoryValue = loadWord(alignedAddress);

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

    storeWord(alignedAddress, memoryValue);
}

void CPU::operationStoreWordRight(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    uint32_t value = registerAtIndex(rt);

    uint32_t alignedAddress = address & !3;
    uint32_t currentMemoryValue = loadWord(alignedAddress);

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

    storeWord(alignedAddress, memoryValue);
}

void CPU::operationLoadWordCoprocessor0(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}

void CPU::operationLoadWordCoprocessor1(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}

void CPU::operationLoadWordCoprocessor2(Instruction instruction) {
    cout << "Unhandled GTE LWC" << hex << instruction.dat() << endl;
    exit(1);
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
    cout << "Unhandled GTE SWC" << hex << instruction.dat() << endl;
    exit(1);
}

void CPU::operationStoreWordCoprocessor3(Instruction instruction) {
    triggerException(ExceptionType::Coprocessor);
}

void CPU::operationIllegal(Instruction instruction) {
    triggerException(ExceptionType::Illegal);
}
