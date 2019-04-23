#pragma once
#include <cstdint>

const uint32_t SCRATCHPAD_SIZE = 1024;

class Scratchpad {
    uint8_t data[SCRATCHPAD_SIZE];
public:
    Scratchpad();
    ~Scratchpad();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);
};
