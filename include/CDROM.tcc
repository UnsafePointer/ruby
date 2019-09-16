#pragma once
#include "CDROM.hpp"
#include "Output.hpp"

template <typename T>
inline T CDROM::load(uint32_t offset) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (sizeof(T) != 1) {
        printError("Unsupported CDROM read with size: %d", sizeof(T));
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
                   printError("Unhandled CDROM read at offset: %#x, with index: %d", offset, status.index);
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
                   printError("Unhandled CDROM read at offset: %#x, with index: %d", offset, status.index);
                   break;
                }
            }
            break;
        }
        default: {
            printError("Unhandled CDROM read at offset: %#x, with index: %d", offset, status.index);
            break;
        }
    }
    return 0;
}

template <typename T>
inline void CDROM::store(uint32_t offset, T value) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (sizeof(T) != 1) {
        printError("Unsupported CDROM write with size: %d", sizeof(T));
    }
    uint8_t param = value & 0xFF;
    switch (offset) {
        case 0: {
            setStatusRegister(param);
            break;
        }
        case 1: {
            execute(value);
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
                default: {
                    printError("Unhandled CDROM write at offset: %#x, with index: %d", offset, status.index);
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
                default: {
                    printError("Unhandled CDROM write at offset: %#x, with index: %d", offset, status.index);
                    break;
                }
            }
            break;
        }
        default: {
            printError("Unhandled CDROM write at offset: %#x, with index: %d", offset, status.index);
            break;
        }
    }
    return;
}

