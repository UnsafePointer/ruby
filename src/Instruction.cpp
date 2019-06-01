#include "Instruction.hpp"
#include "CPU.hpp"

Instruction::Instruction(uint32_t value) : value(value) {

}

uint32_t Instruction::imm() const {
    return value & 0xFFFF;
}

uint32_t Instruction::immSE() const {
    int16_t signedData = value & 0xFFFF;
    return (uint32_t)signedData;
}

uint32_t Instruction::immjump() const {
    return value & 0x3FFFFFF;
}

uint32_t Instruction::copcode() const {
    return rs & 0x1F;
}
