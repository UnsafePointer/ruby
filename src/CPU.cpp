#include "CPU.hpp"

CPU::CPU(Interconnect &interconnect) : programCounter(0xbfc00000), interconnect(interconnect) {

}

CPU::~CPU() {

}

uint32_t CPU::readWord(uint32_t address) const {
    return interconnect.readWord(address);
}

void CPU::executeNext() {
    uint32_t instruction = readWord(programCounter);
    programCounter+=4;
    executeNextInstruction(instruction);
}

void CPU::executeNextInstruction(uint32_t instruction) {
    exit(1);
}
