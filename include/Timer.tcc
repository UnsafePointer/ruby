#pragma once
#include "Timer.hpp"

template <typename T>
inline T Timer::load(uint32_t offset) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");

    switch (offset) {
        case 0: {
            return counterValueRegister();
        }
        case 4: {
            return counterModeRegister();
        }
        case 8: {
            return counterTargetRegister();
        }
        default: {
            logger.logError("Unhandled Timer load at offset: %#x", offset);
            return 0;
        }
    }
}

template <typename T>
inline void Timer::store(uint32_t offset, T value) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");

    switch (offset) {
        case 0: {
            setCounterValueRegister(value);
            return;
        }
        case 4: {
            setCounterModeRegister(value);
            return;
        }
        case 8: {
            setCounterTargetRegister(value);
            return;
        }
        default: {
            logger.logError("Unhandled Timer load at offset: %#x", offset);
            return;
        }
    }
}
