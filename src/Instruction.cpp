#include "Instruction.hpp"

Instruction::Instruction(uint32_t data) : data(data) {

}

Instruction::~Instruction() {

}

uint32_t Instruction::dat() const {
    return data;
}

uint32_t Instruction::funct() const {
    return data >> 26;
}

uint32_t Instruction::subfunct() const {
    return data & 0x3F;
}

uint32_t Instruction::rs() const {
    return (data >> 21) & 0x1F;
}

uint32_t Instruction::rt() const {
    return (data >> 16) & 0x1F;
}

uint32_t Instruction::rd() const {
    return (data >> 11) & 0x1F;
}

uint32_t Instruction::imm() const {
    return data & 0xFFFF;
}

uint32_t Instruction::immSE() const {
    int16_t signedData = data & 0xFFFF;
    return (uint32_t)signedData;
}

uint32_t Instruction::shiftimm() const {
    return (data >> 6) & 0x1F;
}

uint32_t Instruction::immjump() const {
    return data & 0x3FFFFFF;
}
