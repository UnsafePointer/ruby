#pragma once
#include "CPU.hpp"
#include "Interconnect.tcc"

template <typename T>
inline T CPU::load(uint32_t address) const {
    return interconnect->load<T>(address);
}

template <typename T>
inline void CPU::store(uint32_t address, T value) const {
    interconnect->store<T>(address, value);
}
