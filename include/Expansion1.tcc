#pragma once
#include "Expansion1.hpp"

template <typename T>
inline T Expansion1::load(uint32_t offset) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    T value = 0;
    for (uint8_t i = 0; i < sizeof(T); i++) {
        value |= (((uint32_t)data[offset + i]) << (i * 8));
    }
    return value;
}

template <typename T>
inline void Expansion1::store(uint32_t offset, T value) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    for (uint8_t i = 0; i < sizeof(T); i++) {
        data[offset + i] = ((uint8_t)(((uint32_t)value) >> (i * 8)));
    }
}
