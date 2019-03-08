#pragma once
#include "GPU.hpp"
#include <iostream>

template <typename T>
inline T GPU::load(uint32_t offset) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    switch (offset) {
        case 0: {
            return readRegister();
        }
        case 4: {
            return statusRegister();
        }
        default: {
            std::cout << "Unhandled GPU read at offset: 0x" << std::hex << offset << std::endl;
            exit(1);
            return 0;
        }
    }
}

template <typename T>
inline void GPU::store(uint32_t offset, T value) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    switch (offset) {
        case 0: {
            executeGp0(value);
            break;
        }
        case 4: {
            executeGp1(value);
            break;
        }
        default: {
            std::cout << "Unhandled GPU write at offset: 0x" << std::hex << offset << std::endl;
            exit(1);
        }
    }
}
