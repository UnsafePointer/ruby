#pragma once
#include <cstdint>

class SPU {

public:
    SPU();
    ~SPU();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);
};
