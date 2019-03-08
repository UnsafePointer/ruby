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
        return dmaRegister(*offset);
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
