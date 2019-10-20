#include "SPU.hpp"

template <typename T>
inline T SPU::load(uint32_t offset) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    switch (offset) {
        default: {
            logger.logError("Unhandled Sound Processing Unit read at offset: %#x, of size: %d", offset, sizeof(T));
            return 0;
        }
    }
}

template <typename T>
inline void SPU::store(uint32_t offset, T value) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    (void)value;
    switch (offset) {
        case 0x180: {
            logger.logMessage("Unhandled Sound Processing Unit main volume left write");
            break;
        }
        case 0x182: {
            logger.logMessage("Unhandled Sound Processing Unit main volume right write");
            break;
        }
        case 0x184: {
            logger.logMessage("Unhandled Sound Processing Unit reverb volume output left write");
            break;
        }
        case 0x186: {
            logger.logMessage("Unhandled Sound Processing Unit reverb volume output right write");
            break;
        }
        default: {
            logger.logError("Unhandled Sound Processing Unit write at offset: %#x, of size: %d", offset, sizeof(T));
            return;
        }
    }
}
