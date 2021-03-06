#pragma once
#include "CDROM.hpp"

template <typename T>
inline T CDROM::load(uint32_t offset) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (sizeof(T) != 1) {
        logger.logError("Unsupported CDROM read with size: %d", sizeof(T));
    }
    switch (offset) {
        case 0: {
            return getStatusRegister();
        }
        case 1: {
            switch (status.index) {
                case 1: {
                    return getReponse();
                }
                default: {
                   logger.logError("Unhandled CDROM read at offset: %#x, with index: %d", offset, status.index);
                   break;
                }
            }
            break;
        }
        case 3: {
            switch (status.index) {
                case 0: {
                    return getInterruptRegister();
                }
                case 1: {
                    return getInterruptFlagRegister();
                }
                case 2: {
                    return getInterruptRegister();
                }
                case 3: {
                    return getInterruptFlagRegister();
                }
                default: {
                   logger.logError("Unhandled CDROM read at offset: %#x, with index: %d", offset, status.index);
                   break;
                }
            }
            break;
        }
        default: {
            logger.logError("Unhandled CDROM read at offset: %#x, with index: %d", offset, status.index);
            break;
        }
    }
    return 0;
}

template <typename T>
inline void CDROM::store(uint32_t offset, T value) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (sizeof(T) != 1) {
        logger.logError("Unsupported CDROM write with size: %d", sizeof(T));
    }
    uint8_t param = value & 0xFF;
    switch (offset) {
        case 0: {
            setStatusRegister(param);
            break;
        }
        case 1: {
            switch (status.index) {
                case 0: {
                    execute(value);
                    break;
                }
                default: {
                    logger.logWarning("Unhandled CDROM write at offset: %#x, with index: %d", offset, status.index);
                    break;
                }
            }
            break;
        }
        case 2: {
            switch (status.index) {
                case 0: {
                    pushParameter(value);
                    break;
                }
                case 1: {
                    setInterruptRegister(value);
                    break;
                }
                case 2: {
                    setAudioVolumeLeftCDToLeftSPURegister(value);
                    break;
                }
                case 3: {
                    setAudioVolumeRightCDToLeftSPURegister(value);
                    break;
                }
                default: {
                    logger.logError("Unhandled CDROM write at offset: %#x, with index: %d", offset, status.index);
                    break;
                }
            }
            break;
        }
        case 3: {
            switch (status.index) {
                case 0: {
                    setRequestRegister(value);
                    break;
                }
                case 1: {
                    setInterruptFlagRegister(value);
                    break;
                }
                case 2: {
                    setAudioVolumeLeftCDToRightSPURegister(value);
                    break;
                }
                case 3: {
                    logger.logWarning("Unhandled CDROM Audio Volume Apply Changes register write");
                    break;
                }
                default: {
                    logger.logError("Unhandled CDROM write at offset: %#x, with index: %d", offset, status.index);
                    break;
                }
            }
            break;
        }
        default: {
            logger.logError("Unhandled CDROM write at offset: %#x, with index: %d", offset, status.index);
            break;
        }
    }
    return;
}

