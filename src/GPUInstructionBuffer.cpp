#include "GPUInstructionBuffer.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

GPUInstructionBuffer::GPUInstructionBuffer() : logger(LogLevel::NoLog) {

}

GPUInstructionBuffer::~GPUInstructionBuffer() {

}

uint32_t& GPUInstructionBuffer::operator[] (const uint8_t index) {
    if (index >= length) {
        logger.logError("GPU Instruction Buffer index-out-of-bounds: %d", index);
    }
    return buffer[index];
}

void GPUInstructionBuffer::clear() {
    fill_n(buffer, GPU_INSTRUCTION_BUFFER_SIZE, 0x0);
    length = 0;
}

void GPUInstructionBuffer::pushWord(uint32_t value) {
    buffer[length] = value;
    length++;
}
