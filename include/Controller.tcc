#pragma once
#include "Controller.hpp"
#include "Output.hpp"

template <typename T>
inline T Controller::load(uint32_t offset) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (sizeof(T) != 2) {
        printError("Unsupported Controller read with size: %d", sizeof(T));
    }
    switch (offset) {
        default: {
            printError("Unhandled Controller read at offset: %#x", offset);
            break;
        }
    }
    return 0;
}

template <typename T>
inline void Controller::store(uint32_t offset, T value) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (sizeof(T) != 2) {
        printError("Unsupported Controller write with size: %d", sizeof(T));
    }
    switch (offset) {
        case 0x8: {
            setModeRegister(value);
            break;
        }
        case 0xa: {
            setControlRegister(value);
            break;
        }
        case 0xe: {
            setJoypadBaudRegister(value);
            break;
        }
        default: {
            printError("Unhandled Controller write at offset: %#x", offset);
            break;
        }
    }
}
