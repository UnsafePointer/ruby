#include "CPU.hpp"
#include <algorithm>

CPU::CPU(Interconnect &interconnect) : programCounter(0xbfc00000), interconnect(interconnect) {
    std::fill_n(registers, 32, 0xDEADBEEF);
    registers[0] = 0;
}

CPU::~CPU() {

}

uint32_t CPU::readWord(uint32_t address) const {
    return interconnect.readWord(address);
}

void CPU::executeNext() {
    uint32_t data = readWord(programCounter);
    Instruction instruction = Instruction(data);
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
        case 0b001111: {
            operationLoadUpperImmediate(instruction);
            break;
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
