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
#include "Timer.tcc"
#include "Controller.tcc"

template <typename T>
inline T Interconnect::load(uint32_t address) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    uint32_t absoluteAddress = maskRegion(address);

    std::optional<Range> range = findRangeForAddress(absoluteAddress, &ranges, 0, ranges.size());
    if (range == std::nullopt) {
        printWarning("Unhandled read at: %#x", address);
        Debugger *debugger = Debugger::getInstance();
        if (debugger->isAttached()) {
            return 0;
        }
        exit(1);
    }
    uint32_t offset = (*range).offset(absoluteAddress);
    switch ((*range).identifier) {
        case RAMR: {
            return ram->load<T>(offset);
        }
        case Expansion1R: {
            return expansion1->load<T>(offset);
        }
        case ScratchpadR: {
            return scratchpad->load<T>(offset);
        }
        case MemoryControlR: {
            printError("Unhandled Memory Control read at offset: %#x", offset);
            return 0;
        }
        case ControllerRegisterR: {
            return controller->load<T>(offset);
        }
        case RamSizeR: {
            printError("Unhandled RAM size read at offset: %#x", offset);
            return 0;
        }
        case InterruptRequestControlR: {
            return interruptController->load<T>(offset);
        }
        case DMARegisterR: {
            return dma->load<T>(offset);
        }
        case Timer0RegisterR: {
            return timer0->load<T>(offset);
        }
        case Timer1RegisterR: {
            return timer1->load<T>(offset);
        }
        case Timer2RegisterR: {
            return timer2->load<T>(offset);
        }
        case CDROMRegisterR: {
            return cdrom->load<T>(offset);
        }
        case GPURegisterR: {
            return gpu->load<T>(offset);
        }
        case SoundProcessingUnitR: {
            printMessage("Unhandled Sound Processing Unit read at offset: %#x", offset);
            return 0;
        }
        case Expansion2R: {
            printError("Unhandled Expansion 2 read at offset: %#x", offset);
            return 0;
        }
        case BIOSR: {
            return bios->load<T>(offset);
        }
        case CacheControlR: {
            printError("Unhandled cache control read at offset: %#x", offset);
            return 0;
        }
    }
}

template <typename T>
inline void Interconnect::store(uint32_t address, T value) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (address % sizeof(T) != 0) {
        printError("Unaligned memory store");
        exit(1);
    }
    uint32_t absoluteAddress = maskRegion(address);
    std::optional<Range> range = findRangeForAddress(absoluteAddress, &ranges, 0, ranges.size());
    if (range == std::nullopt) {
        printError("Unhandled write at: %#x", address);
    }
    uint32_t offset = (*range).offset(absoluteAddress);
    switch ((*range).identifier) {
        case RAMR: {
            if (cop0->isCacheIsolated()) {
                return;
            }
            ram->store<T>(offset, value);
            return;
        }
        case Expansion1R: {
            printMessage("Unhandled Expansion 1 write at offset: %#x", offset);
            return;
        }
        case ScratchpadR: {
            scratchpad->store<T>(offset, value);
            return;
        }
        case MemoryControlR: {
            // PlayStation BIOS should not set these to any different value
            switch (offset) {
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
                    printWarning("Unhandled Memory Control write at offset: %#x", offset);
                    break;
                }
            }
            return;
        }
        case ControllerRegisterR: {
            controller->store<T>(offset, value);
            return;
        }
        case RamSizeR: {
            printWarning("Unhandled RAM Control write at offset: %#x", offset);
            return;
        }
        case InterruptRequestControlR: {
            interruptController->store<T>(offset, value);
            return;
        }
        case DMARegisterR: {
            dma->store<T>(offset, value);
            return;
        }
        case Timer0RegisterR: {
            timer0->store<T>(offset, value);
            return;
        }
        case Timer1RegisterR: {
            timer1->store<T>(offset, value);
            return;
        }
        case Timer2RegisterR: {
            timer2->store<T>(offset, value);
            return;
        }
        case CDROMRegisterR: {
            cdrom->store<T>(offset, value);
            return;
        }
        case GPURegisterR: {
            gpu->store<T>(offset, value);
            return;
        }
        case SoundProcessingUnitR: {
            printMessage("Unhandled Sound Processing Unit write at offset: %#x", offset);
            return;
        }
        case Expansion2R: {
            printWarning("Unhandled Expansion 2 write at offset: %#x", offset);
            return;
        }
        case BIOSR: {
            printError("Unhandled BIOS write at offset: %#x", offset);
            return;
        }
        case CacheControlR: {
            printWarning("Unhandled Cache Control write at offset: %#x", offset);
            return;
        }
    }
}
