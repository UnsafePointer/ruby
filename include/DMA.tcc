#pragma once
#include "DMA.hpp"
#include <iostream>

template <typename T>
inline T DMA::load(uint32_t offset) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
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
                    std::cout << "Unhandled DMA access at offset: 0x" << std::hex << offset << std::endl;
                    exit(1);
                }
            }
        }
        case 7: {
            switch (lower) {
                case 0: {
                    return ctrlRegister();
                }
                case 4: {
                    return interruptRegister();
                }
                default: {
                    std::cout << "Unhandled DMA access at offset: 0x" << std::hex << offset << std::endl;
                    exit(1);
                }
            }
        }
        default: {
            std::cout << "Unhandled DMA access at offset: 0x" << std::hex << offset << std::endl;
            exit(1);
        }
    }
}

template <typename T>
inline void DMA::store(uint32_t offset, T value) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
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
                    std::cout << "Unhandled DMA write access at offset: 0x" << std::hex << offset << std::endl;
                    exit(1);
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
                    std::cout << "Unhandled DMA write access at offset: 0x" << std::hex << offset << std::endl;
                    exit(1);
                }
            }
            break;
        }
        default: {
            std::cout << "Unhandled DMA write access at offset: 0x" << std::hex << offset << std::endl;
            exit(1);
        }
    }
    if (activePort != Port::None) {
        execute(activePort);
    }
}
