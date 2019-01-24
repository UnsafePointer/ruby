#pragma once
#include <cstdint>

struct Instruction {
    const uint32_t data;

    Instruction(uint32_t data);
    ~Instruction();
    uint32_t funct() const;
    uint32_t rt() const;
    uint32_t imm() const;
};
