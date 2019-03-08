#pragma once
#include "Interconnect.hpp"
#include <iostream>
#include "RAM.tcc"
#include "BIOS.tcc"
#include "GPU.tcc"

template <typename T>
inline T Interconnect::load(uint32_t address) const {
    uint32_t absoluteAddress = maskRegion(address);

    std::optional<uint32_t> offset = biosRange.contains(absoluteAddress);
    if (offset) {
        return bios->load<T>(*offset);
    }
    offset = ramRange.contains(absoluteAddress);
    if (offset) {
        return ram->load<T>(*offset);
    }
    offset = interruptRequestControlRange.contains(absoluteAddress);
    if (offset) {
        std::cout << "Unhandled Interrupt Request Control read at offset: 0x" << std::hex << *offset << std::endl;
        return 0;
    }
    offset = dmaRegisterRange.contains(absoluteAddress);
    if (offset) {
        return dma->dmaRegister(*offset);
    }
    offset = gpuRegisterRange.contains(absoluteAddress);
    if (offset) {
        return gpu->load<T>(*offset);
    }
    offset = timerRegisterRange.contains(absoluteAddress);
    if (offset) {
        std::cout << "Unhandled Timer Register read at offset: 0x" << std::hex << *offset << std::endl;
        return 0;
    }
    offset = expansion1Range.contains(absoluteAddress);
    if (offset) {
        // No expansion
        return 0xFF;
    }
    offset = soundProcessingUnitRange.contains(absoluteAddress);
    if (offset) {
        std::cout << "Unhandled Sound Processing Unit read at offset: 0x" << std::hex << *offset << std::endl;
        return 0;
    }
    std::cout << "Unhandled read at: 0x" << std::hex << address << std::endl;
    exit(1);
}

template <typename T>
inline void Interconnect::store(uint32_t address, T value) const {
    if (address % sizeof(T) != 0) {
        std::cout << "Unaligned memory store" << std::endl;
        exit(1);
    }
    uint32_t absoluteAddress = maskRegion(address);
    std::optional<uint32_t> offset;
    offset = memoryControlRange.contains(absoluteAddress);
    if (offset) {
        // PlayStation BIOS should not set these to any different value
        switch (*offset) {
            case 0: {
                if (value != 0x1f000000) {
                    exit(1);
                }
                break;
            }
            case 4: {
                if (value != 0x1f802000) {
                    exit(1);
                }
                break;
            }
            default: {
                std::cout << "Unhandled Memory Control write at offset: 0x" << std::hex << *offset << std::endl;
                break;
            }
        }
        return;
    }
    offset = ramSizeRange.contains(absoluteAddress);
    if (offset) {
        std::cout << "Unhandled RAM Control write at offset: 0x" << std::hex << *offset << std::endl;
        return;
    }
    offset = cacheControlRange.contains(absoluteAddress);
    if (offset) {
        std::cout << "Unhandled Cache Control write at offset: 0x" << std::hex << *offset << std::endl;
        return;
    }
    offset = ramRange.contains(absoluteAddress);
    if (offset) {
        ram->store<T>(*offset, value);
        return;
    }
    offset = interruptRequestControlRange.contains(absoluteAddress);
    if (offset) {
        std::cout << "Unhandled Interrupt Request Control write at offset: 0x" << std::hex << *offset << std::endl;
        return;
    }
    offset = dmaRegisterRange.contains(absoluteAddress);
    if (offset) {
        dma->setDMARegister(*offset, value);
        return;
    }
    offset = gpuRegisterRange.contains(absoluteAddress);
    if (offset) {
        switch (*offset) {
            case 0: {
                gpu->executeGp0(value);
                break;
            }
            case 4: {
                gpu->executeGp1(value);
                break;
            }
            default: {
                std::cout << "Unhandled GPU write at offset: 0x" << std::hex << *offset << std::endl;
                exit(1);
            }
        }
        return;
    }
    offset = timerRegisterRange.contains(absoluteAddress);
    if (offset) {
        std::cout << "Unhandled Timer Register write at offset: 0x" << std::hex << *offset << std::endl;
        return;
    }
    offset = soundProcessingUnitRange.contains(absoluteAddress);
    if (offset) {
        std::cout << "Unhandled Sound Processing Unit write at offset: 0x" << std::hex << *offset << std::endl;
        return;
    }
    offset = expansion2Range.contains(absoluteAddress);
    if (offset) {
        std::cout << "Unhandled Expansion 2 write at offset: 0x" << std::hex << *offset << std::endl;
        return;
    }
    std::cout << "Unhandled write at: 0x" << std::hex << address << std::endl;
    exit(1);
}
