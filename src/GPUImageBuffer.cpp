#include "GPUImageBuffer.hpp"
#include <algorithm>

using namespace std;

GPUImageBuffer::GPUImageBuffer() : destinationX(0), destinationY(0), width(0), heigth(0), index(0) {
    fill_n(buffer, VRAM_SIZE, 0x0);
}

GPUImageBuffer::~GPUImageBuffer() {

}

std::pair<uint16_t, uint16_t> GPUImageBuffer::destination() {
    return {destinationX, destinationY};
}

std::pair<uint16_t, uint16_t> GPUImageBuffer::resolution() {
    return {width, heigth};
}

void GPUImageBuffer::reset(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    index = 0;
    destinationX = x;
    destinationY = y;
    width = w;
    heigth = h;
}

void GPUImageBuffer::pushWord(uint32_t word) {
    buffer[index] = word & 0xffff;
    index++;
    buffer[index] = (word >> 16) & 0xffff;
    index++;
}

bool GPUImageBuffer::isValid() {
    uint32_t resolution = width * heigth;
    return resolution == index;
}

uint16_t* GPUImageBuffer::bufferRef() {
    return buffer;
}
