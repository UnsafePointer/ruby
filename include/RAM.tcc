#pragma once
#include "RAM.hpp"

template <typename T>
inline T RAM::load(uint32_t offset) const {
    T value = 0;
    for (uint8_t i = 0; i < sizeof(T); i++) {
        value |= (((uint32_t)data[offset + i]) << (i * 8));
    }
    return value;
}
