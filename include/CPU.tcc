#pragma once
#include "CPU.hpp"
#include "Interconnect.tcc"

template <typename T>
inline T CPU::load(uint32_t address) const {
    return interconnect->load<T>(address);
}
