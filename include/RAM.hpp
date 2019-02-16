#pragma once
#include <cstdint>

const uint32_t RAM_SIZE = 2*1024*1024;

class RAM {
    uint8_t data[RAM_SIZE];
public:
    RAM();
    ~RAM();

    uint32_t readWord(uint32_t offset) const;
    void storeWord(uint32_t offset, uint32_t value);
    uint8_t readByte(uint32_t offset) const;
    void storeByte(uint32_t offset, uint8_t value);
};
