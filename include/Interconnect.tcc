#pragma once
#include "Interconnect.hpp"
#include "Output.hpp"
#include "RAM.tcc"
#include "BIOS.tcc"
#include "GPU.tcc"
#include "DMA.tcc"
#include "Debugger.hpp"
#include "Scratchpad.tcc"
#include "CDROM.tcc"
#include "InterruptController.tcc"
#include "Expansion1.tcc"

template <typename T>
inline T Interconnect::load(uint32_t address) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
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
        return interruptController->load<T>(*offset);
    }
    offset = dmaRegisterRange.contains(absoluteAddress);
    if (offset) {
        return dma->load<T>(*offset);
    }
    offset = gpuRegisterRange.contains(absoluteAddress);
    if (offset) {
        return gpu->load<T>(*offset);
    }
    offset = timerRegisterRange.contains(absoluteAddress);
    if (offset) {
        printWarning("Unhandled Timer Register read at offset: %#x", *offset);
        return 0;
    }
    offset = expansion1Range.contains(absoluteAddress);
    if (offset) {
        return expansion1->load<T>(*offset);
    }
    offset = soundProcessingUnitRange.contains(absoluteAddress);
    if (offset) {
        printWarning("Unhandled Sound Processing Unit read at offset: %#x", *offset);
        return 0;
    }
    offset = scratchpadRange.contains(absoluteAddress);
    if (offset) {
        return scratchpad->load<T>(*offset);
    }
    offset = cdromRegisterRange.contains(absoluteAddress);
    if (offset) {
        return cdrom->load<T>(*offset);
    }
    printWarning("Unhandled read at: %#x", address);
    Debugger *debugger = Debugger::getInstance();
    if (debugger->isAttached()) {
        return 0;
    }
    exit(1);
}

template <typename T>
inline void Interconnect::store(uint32_t address, T value) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (address % sizeof(T) != 0) {
        printError("Unaligned memory store");
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
                printWarning("Unhandled Memory Control write at offset: %#x", *offset);
                break;
            }
        }
        return;
    }
    offset = ramSizeRange.contains(absoluteAddress);
    if (offset) {
        printWarning("Unhandled RAM Control write at offset: %#x", *offset);
        return;
    }
    offset = cacheControlRange.contains(absoluteAddress);
    if (offset) {
        printWarning("Unhandled Cache Control write at offset: %#x", *offset);
        return;
    }
    offset = ramRange.contains(absoluteAddress);
    if (offset) {
        ram->store<T>(*offset, value);
        return;
    }
    offset = interruptRequestControlRange.contains(absoluteAddress);
    if (offset) {
        interruptController->store<T>(*offset, value);
        return;
    }
    offset = dmaRegisterRange.contains(absoluteAddress);
    if (offset) {
        dma->store<T>(*offset, value);
        return;
    }
    offset = gpuRegisterRange.contains(absoluteAddress);
    if (offset) {
        gpu->store<T>(*offset, value);
        return;
    }
    offset = timerRegisterRange.contains(absoluteAddress);
    if (offset) {
        printWarning("Unhandled Timer Register write at offset: %#x", *offset);
        return;
    }
    offset = soundProcessingUnitRange.contains(absoluteAddress);
    if (offset) {
        printWarning("Unhandled Sound Processing Unit write at offset: %#x", *offset);
        return;
    }
    offset = expansion2Range.contains(absoluteAddress);
    if (offset) {
        printWarning("Unhandled Expansion 2 write at offset: %#x", *offset);
        return;
    }
    offset = scratchpadRange.contains(absoluteAddress);
    if (offset) {
        scratchpad->store<T>(*offset, value);
        return;
    }
    offset = cdromRegisterRange.contains(absoluteAddress);
    if (offset) {
        cdrom->store<T>(*offset, value);
        return;
    }
    printError("Unhandled write at: %#x", address);
}
