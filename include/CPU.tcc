#pragma once
#include "CPU.hpp"
#include "Interconnect.tcc"

template <typename T>
inline T CPU::load(uint32_t address) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    return interconnect->load<T>(address);
}

template <typename T>
inline void CPU::store(uint32_t address, T value) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    interconnect->store<T>(address, value);
}
