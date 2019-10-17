#include "InterruptController.hpp"
#include "Output.hpp"

template <typename T>
inline T InterruptController::load(uint32_t offset) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    switch (offset) {
        case 0: {
            return status.value;
        }
        case 4: {
            return mask.value;
        }
        default: {
            logger.logError(format("Unhandled Interrupt Request Control read at offset: %#x", offset));
            return 0;
        }
    }
}

template <typename T>
inline void InterruptController::store(uint32_t offset, T value) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    switch (offset) {
        case 0: {
            setStatus(value);
            return;
        }
        case 4: {
            setMask(value);
            return;
        }
        default: {
            logger.logError(format("Unhandled Interrupt Request Control write at offset: %#x", offset));
            return;
        }
    }
}
