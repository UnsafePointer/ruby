#pragma once
#include <cstdint>

const uint32_t RAM_SIZE = 2*1024*1024;

class RAM {
    uint8_t data[RAM_SIZE];
public:
    RAM();
    ~RAM();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);
};
