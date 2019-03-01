#include "GPUInstructionBuffer.hpp"
#include <iostream>
#include <algorithm>

using namespace std;

GPUInstructionBuffer::GPUInstructionBuffer() {

}

GPUInstructionBuffer::~GPUInstructionBuffer() {

}

uint32_t& GPUInstructionBuffer::operator[] (const uint8_t index) {
    if (index >= length) {
        cout << "GPU Instruction Buffer index-out-of-bounds: " << dec << index << endl;
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
