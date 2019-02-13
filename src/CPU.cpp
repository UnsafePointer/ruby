#include "CPU.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

CPU::CPU(Interconnect &interconnect) : programCounter(0xbfc00000), nextInstruction(Instruction(0x0)), statusRegister(0), interconnect(interconnect) {
    std::fill_n(registers, 32, 0xDEADBEEF);
    registers[0] = 0;
}

CPU::~CPU() {

}

uint32_t CPU::readWord(uint32_t address) const {
    return interconnect.readWord(address);
}

void CPU::storeWord(uint32_t address, uint32_t value) const {
    if ((statusRegister & 0x10000) != 0) {
        cout << "Cache is isolated, ignoring store at address: 0x" << hex << address << endl;
        return;
    }
    return interconnect.storeWord(address, value);
}

void CPU::executeNextInstruction() {
    Instruction instruction = nextInstruction;
    uint32_t data = readWord(programCounter);
    nextInstruction = Instruction(data);
    programCounter+=4;
    executeNextInstruction(instruction);
}

uint32_t CPU::registerAtIndex(RegisterIndex index) const {
    return registers[index.idx()];
}

void CPU::setRegisterAtIndex(RegisterIndex index, uint32_t value) {
    registers[index.idx()] = value;

    // Make sure R0 is always 0
    registers[0] = 0;
}

void CPU::executeNextInstruction(Instruction instruction) {
    switch (instruction.funct()) {
        case 0b000000: {
            switch (instruction.subfunct()) {
                case 0b000000: {
                    operationShiftLeftLogical(instruction);
                    break;
                }
                case 0b100101: {
                    operationOr(instruction);
                    break;
                }
                default: {
                    cout << "Unhandled instruction 0x" << hex << instruction.dat() << endl;
                    exit(1);
                }
            }
        }
        case 0b001111: {
            operationLoadUpperImmediate(instruction);
            break;
        }
        case 0b001101: {
            operationBitwiseOrImmediate(instruction);
            break;
        }
        case 0b101011: {
            operationStoreWord(instruction);
            break;
        }
        case 0b01001: {
            operationAddImmediateUnsigned(instruction);
            break;
        }
        case 0b000010: {
            operationJump(instruction);
            break;
        }
        case 0b010000: {
            operationCoprocessor0(instruction);
            break;
        }
        default: {
            cout << "Unhandled instruction 0x" << hex << instruction.dat() << endl;
            exit(1);
        }
    }
}

void CPU::operationCoprocessor0(Instruction instruction) {
    switch (instruction.copcode()) {
        case 0b00100: {
            operationMoveToCoprocessor0(instruction);
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
        case 12: {
            statusRegister = value;
            break;
        }
        default: {
            cout << "Unhandled MTC coprocessor0 index " << copRegisterIndex.idx() << endl;
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

void CPU::operationStoreWord(Instruction instruction) const {
    uint32_t imm = instruction.immSE();
    RegisterIndex rt = instruction.rt();
    RegisterIndex rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
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
    programCounter = (programCounter & 0xF0000000) | (imm << 2);
}

void CPU::operationOr(Instruction instruction) {
    RegisterIndex rd = instruction.rd();
    RegisterIndex rs = instruction.rs();
    RegisterIndex rt = instruction.rt();

    uint32_t value = registerAtIndex(rs) | registerAtIndex(rt);
    setRegisterAtIndex(rd, value);
}
