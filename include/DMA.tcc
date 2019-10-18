#pragma once
#include "DMA.hpp"

template <typename T>
inline T DMA::load(uint32_t offset) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (sizeof(T) != 4) {
        logger.logError("Unsupported DMA read with size: %d", sizeof(T));
    }
    uint32_t upper = (offset & 0x70) >> 4;
    uint32_t lower = (offset & 0xf);
    switch (upper) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6: {
            Port port = portWithIndex(upper);
            Channel channel = channelForPort(port);
            switch (lower) {
                case 0: {
                    return channel.baseAddressRegister();
                }
                case 4: {
                    return channel.blockControlRegister();
                }
                case 8: {
                    return channel.controlRegister();
                }
                default: {
                    logger.logError("Unhandled DMA read at offset: %#x", offset);
                    return 0;
                }
            }
        }
        case 7: {
            switch (lower) {
                case 0: {
                    return controlRegister();
                }
                case 4: {
                    return interruptRegister();
                }
                default: {
                    logger.logError("Unhandled DMA read at offset: %#x", offset);
                    return 0;
                }
            }
        }
        default: {
            logger.logError("Unhandled DMA read at offset: %#x", offset);
            return 0;
        }
    }
}

template <typename T>
inline void DMA::store(uint32_t offset, T value) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (sizeof(T) != 4) {
        logger.logError("Unsupported DMA write with size: %d", sizeof(T));
    }
    uint32_t upper = (offset & 0x70) >> 4;
    uint32_t lower = (offset & 0xf);
    Port activePort = Port::None;
    switch (upper) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6: {
            Port port = portWithIndex(upper);
            Channel& channel = channelForPort(port);
            switch (lower) {
                case 0: {
                    channel.setBaseAddressRegister(value);
                    break;
                }
                case 4: {
                    channel.setBlockControlRegister(value);
                    break;
                }
                case 8: {
                    channel.setControlRegister(value);
                    break;
                }
                default: {
                    logger.logError("Unhandled DMA write at offset: %#x", offset);
                    return;
                }
            }
            if (channel.isActive()) {
                activePort = port;
            }
            break;
        }
        case 7: {
            switch (lower) {
                case 0: {
                    setControlRegister(value);
                    break;
                }
                case 4: {
                    setInterruptRegister(value);
                    break;
                }
                default: {
                    logger.logError("Unhandled DMA write at offset: %#x", offset);
                    return;
                }
            }
            break;
        }
        default: {
            logger.logError("Unhandled DMA write at offset: %#x", offset);
            return;
        }
    }
    if (activePort != Port::None) {
        execute(activePort);
    }
}
