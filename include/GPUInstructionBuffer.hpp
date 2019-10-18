#pragma once
#include <cstdint>
#include "Logger.hpp"

const uint32_t GPU_INSTRUCTION_BUFFER_SIZE = 12;

class GPUInstructionBuffer {
    Logger logger;
    uint32_t buffer[GPU_INSTRUCTION_BUFFER_SIZE];
    uint8_t length;
public:
    GPUInstructionBuffer();
    ~GPUInstructionBuffer();

    uint32_t& operator[] (const uint8_t index);
    void clear();
    void pushWord(uint32_t value);
};
