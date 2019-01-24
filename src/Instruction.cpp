#include "Instruction.hpp"

Instruction::Instruction(uint32_t data) : data(data) {

}

Instruction::~Instruction() {

}

uint32_t Instruction::funct() const {
    return data >> 26;
}

uint32_t Instruction::rt() const {
    return (data >> 16) & 0x1F;
}

uint32_t Instruction::imm() const {
    return data & 0xFFFF;
}
