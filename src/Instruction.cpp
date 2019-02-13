#include "Instruction.hpp"
#include "CPU.hpp"

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

RegisterIndex Instruction::rs() const {
    return RegisterIndex((data >> 21) & 0x1F);
}

RegisterIndex Instruction::rt() const {
    return RegisterIndex((data >> 16) & 0x1F);
}

RegisterIndex Instruction::rd() const {
    return RegisterIndex((data >> 11) & 0x1F);
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

uint32_t Instruction::copcode() const {
    return (data >> 21) & 0x1F;
}
