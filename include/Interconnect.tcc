#pragma once
#include "Interconnect.hpp"
#include "RAM.tcc"
#include "BIOS.tcc"
#include "GPU.tcc"
#include "DMA.tcc"
#include "Debugger.hpp"
#include "Scratchpad.tcc"
#include "CDROM.tcc"
#include "InterruptController.tcc"
#include "Expansion1.tcc"
#include "Timer.tcc"
#include "Controller.tcc"
#include "SPU.tcc"

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
    offset = timer0RegisterRange.contains(absoluteAddress);
    if (offset) {
        return timer0->load<T>(*offset);
    }
    offset = timer1RegisterRange.contains(absoluteAddress);
    if (offset) {
        return timer1->load<T>(*offset);
    }
    offset = timer2RegisterRange.contains(absoluteAddress);
    if (offset) {
        return timer2->load<T>(*offset);
    }
    offset = expansion1Range.contains(absoluteAddress);
    if (offset) {
        return expansion1->load<T>(*offset);
    }
    offset = soundProcessingUnitRange.contains(absoluteAddress);
    if (offset) {
        return spu->load<T>(*offset);
    }
    offset = scratchpadRange.contains(absoluteAddress);
    if (offset) {
        return scratchpad->load<T>(*offset);
    }
    offset = cdromRegisterRange.contains(absoluteAddress);
    if (offset) {
        return cdrom->load<T>(*offset);
    }
    offset = controllerRegisterRange.contains(absoluteAddress);
    if (offset) {
        return controller->load<T>(*offset);
    }
    offset = memoryControlRange.contains(absoluteAddress);
    if (offset) {
        logger.logWarning("Unhandled Memory Control read at offset: %#x", *offset);
        return 0;
    }
    offset = ramSizeRange.contains(absoluteAddress);
    if (offset) {
        logger.logWarning("Unhandled RAM Control read at offset: %#x", *offset);
        return 0;
    }
    Debugger *debugger = Debugger::getInstance();
    if (debugger->isAttached()) {
        return 0;
    }
    logger.logError("Unhandled read at: %#x", address);
    return 0;
}

template <typename T>
inline void Interconnect::store(uint32_t address, T value) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (address % sizeof(T) != 0) {
        logger.logError("Unaligned memory store");
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
                logger.logWarning("Unhandled Memory Control write at offset: %#x", *offset);
                break;
            }
        }
        return;
    }
    offset = ramSizeRange.contains(absoluteAddress);
    if (offset) {
        logger.logWarning("Unhandled RAM Control write at offset: %#x", *offset);
        return;
    }
    offset = cacheControlRange.contains(absoluteAddress);
    if (offset) {
        logger.logWarning("Unhandled Cache Control write at offset: %#x", *offset);
        return;
    }
    offset = ramRange.contains(absoluteAddress);
    if (offset) {
        if (cop0->isCacheIsolated()) {
            return;
        }
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
    offset = timer0RegisterRange.contains(absoluteAddress);
    if (offset) {
        timer0->store<T>(*offset, value);
        return;
    }
    offset = timer1RegisterRange.contains(absoluteAddress);
    if (offset) {
        timer1->store<T>(*offset, value);
        return;
    }
    offset = timer2RegisterRange.contains(absoluteAddress);
    if (offset) {
        timer2->store<T>(*offset, value);
        return;
    }
    offset = expansion1Range.contains(absoluteAddress);
    if (offset) {
        expansion1->store<T>(*offset, value);
        return;
    }
    offset = soundProcessingUnitRange.contains(absoluteAddress);
    if (offset) {
        spu->store<T>(*offset, value);
        return;
    }
    offset = expansion2Range.contains(absoluteAddress);
    if (offset) {
        logger.logWarning("Unhandled Expansion 2 write at offset: %#x", *offset);
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
    offset = controllerRegisterRange.contains(absoluteAddress);
    if (offset) {
        controller->store<T>(*offset, value);
        return;
    }
    logger.logError("Unhandled write at: %#x", address);
}
