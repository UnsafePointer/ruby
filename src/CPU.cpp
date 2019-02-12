#include "CPU.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

CPU::CPU(Interconnect &interconnect) : programCounter(0xbfc00000), nextInstruction(Instruction(0x0)), interconnect(interconnect) {
    std::fill_n(registers, 32, 0xDEADBEEF);
    registers[0] = 0;
}

CPU::~CPU() {

}

uint32_t CPU::readWord(uint32_t address) const {
    return interconnect.readWord(address);
}

void CPU::storeWord(uint32_t address, uint32_t value) const {
    return interconnect.storeWord(address, value);
}

void CPU::executeNextInstruction() {
    Instruction instruction = nextInstruction;
    uint32_t data = readWord(programCounter);
    nextInstruction = Instruction(data);
    programCounter+=4;
    executeNextInstruction(instruction);
}

uint32_t CPU::registerAtIndex(uint8_t index) const {
    return registers[index];
}

void CPU::setRegisterAtIndex(uint8_t index, uint32_t value) {
    registers[index] = value;

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
        default: {
            cout << "Unhandled instruction 0x" << hex << instruction.dat() << endl;
            exit(1);
        }
    }
}

void CPU::operationLoadUpperImmediate(Instruction instruction) {
    uint32_t imm = instruction.imm();
    uint32_t rt = instruction.rt();

    uint32_t value = imm << 16;
    setRegisterAtIndex(rt, value);
}

void CPU::operationBitwiseOrImmediate(Instruction instruction) {
    uint32_t imm = instruction.imm();
    uint32_t rt = instruction.rt();
    uint32_t rs = instruction.rs();

    uint32_t value = registerAtIndex(rs) | imm;

    setRegisterAtIndex(rt, value);
}

void CPU::operationStoreWord(Instruction instruction) const {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt();
    uint32_t rs = instruction.rs();

    uint32_t address = registerAtIndex(rs) + imm;
    uint32_t value = registerAtIndex(rt);

    storeWord(address, value);
}

void CPU::operationShiftLeftLogical(Instruction instruction) {
    uint32_t imm = instruction.shiftimm();
    uint32_t rt = instruction.rt();
    uint32_t rd = instruction.rd();

    uint32_t value = registerAtIndex(rt) << imm;
    setRegisterAtIndex(rd, value);
}

void CPU::operationAddImmediateUnsigned(Instruction instruction) {
    uint32_t imm = instruction.immSE();
    uint32_t rt = instruction.rt();
    uint32_t rs = instruction.rs();

    uint32_t value = registerAtIndex(rs) + imm;
    setRegisterAtIndex(rt, value);
}

void CPU::operationJump(Instruction instruction) {
    uint32_t imm = instruction.immjump();
    programCounter = (programCounter & 0xF0000000) | (imm << 2);
}
