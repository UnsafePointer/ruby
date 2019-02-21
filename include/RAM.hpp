#pragma once
#include <cstdint>

const uint32_t RAM_SIZE = 2*1024*1024;

class RAM {
    uint8_t data[RAM_SIZE];
public:
    RAM();
    ~RAM();

    uint32_t loadWord(uint32_t offset) const;
    uint16_t loadHalfWord(uint32_t offset) const;
    uint8_t loadByte(uint32_t offset) const;
    void storeWord(uint32_t offset, uint32_t value);
    void storeHalfWord(uint32_t offset, uint16_t value);
    void storeByte(uint32_t offset, uint8_t value);
};
