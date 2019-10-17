#pragma once
#include "GPU.hpp"
#include "Output.hpp"

template <typename T>
inline T GPU::load(uint32_t offset) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (sizeof(T) != 4) {
        logger.logError(format("Unsupported GPU read with size: %d", sizeof(T)));
    }
    switch (offset) {
        case 0: {
            return readRegister();
        }
        case 4: {
            return statusRegister();
        }
        default: {
            logger.logError(format("Unhandled GPU read at offset: %#x", offset));
            return 0;
        }
    }
}

template <typename T>
inline void GPU::store(uint32_t offset, T value) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (sizeof(T) != 4) {
        logger.logError(format("Unsupported GPU write with size: %d", sizeof(T)));
    }
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
            logger.logError(format("Unhandled GPU write at offset: %#x", offset));
            return;
        }
    }
}
