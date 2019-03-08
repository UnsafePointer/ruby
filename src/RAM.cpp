#include "RAM.hpp"
#include <algorithm>

using namespace std;

RAM::RAM() : data() {
    fill_n(data, RAM_SIZE, 0xCA);
}

RAM::~RAM() {

}

void RAM::storeWord(uint32_t offset, uint32_t value) {
    uint8_t b0 = value & 0xFF;
    uint8_t b1 = (value & 0xFF00) >> 8;
    uint8_t b2 = (value & 0xFF0000) >> 16;
    uint8_t b3 = (value & 0xFF000000) >> 24;

    data[offset + 0] = b0;
    data[offset + 1] = b1;
    data[offset + 2] = b2;
    data[offset + 3] = b3;
}

void RAM::storeHalfWord(uint32_t offset, uint16_t value) {
    uint8_t b0 = value & 0xFF;
    uint8_t b1 = (value & 0xFF00) >> 8;

    data[offset + 0] = b0;
    data[offset + 1] = b1;
}

void RAM::storeByte(uint32_t offset, uint8_t value) {
    data[offset] = value;
}
