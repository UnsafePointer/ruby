#include "Interconnect.hpp"
#include "Range.hpp"
#include <iostream>

using namespace std;

const uint32_t regionMask[8] = {
    // KUSEG: 2048MB
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    // KSEG0: 512MB
    0x7fffffff,
    // KSEG1: 512MB
    0x1fffffff,
    // KSEG2: 1024MB
    0xffffffff, 0xffffffff,
};

Interconnect::Interconnect() {
    bios = make_unique<BIOS>();
    bios->loadBin("SCPH1001.BIN");
    ram = make_unique<RAM>();
    gpu = make_unique<GPU>();
    dma = make_unique<DMA>(ram, gpu);
}

Interconnect::~Interconnect() {
}

uint32_t Interconnect::maskRegion(uint32_t address) const {
    uint8_t index = address >> 29;
    return address & regionMask[index];
}

void Interconnect::storeWord(uint32_t address, uint32_t value) const {
    if (address % 4 != 0) {
        exit(1);
    }
    uint32_t absoluteAddress = maskRegion(address);
    optional<uint32_t> offset;
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
                cout << "Unhandled Memory Control write at offset: 0x" << hex << *offset << endl;
                break;
            }
        }
        return;
    }
    offset = ramSizeRange.contains(absoluteAddress);
    if (offset) {
        cout << "Unhandled RAM Control write at offset: 0x" << hex << *offset << endl;
        return;
    }
    offset = cacheControlRange.contains(absoluteAddress);
    if (offset) {
        cout << "Unhandled Cache Control write at offset: 0x" << hex << *offset << endl;
        return;
    }
    offset = ramRange.contains(absoluteAddress);
    if (offset) {
        ram->storeWord(*offset, value);
        return;
    }
    offset = interruptRequestControlRange.contains(absoluteAddress);
    if (offset) {
        cout << "Unhandled Interrupt Request Control write at offset: 0x" << hex << *offset << endl;
        return;
    }
    offset = dmaRegisterRange.contains(absoluteAddress);
    if (offset) {
        setDMARegister(*offset, value);
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
                cout << "Unhandled GPU write at offset: 0x" << hex << *offset << endl;
                exit(1);
            }
        }
        return;
    }
    offset = timerRegisterRange.contains(absoluteAddress);
    if (offset) {
        cout << "Unhandled Timer Register write at offset: 0x" << hex << *offset << endl;
        return;
    }
    cout << "Unhandled write at: 0x" << hex << address << endl;
    exit(1);
}

void Interconnect::storeHalfWord(uint32_t address, uint16_t value) const {
    if (address % 2 != 0) {
        exit(1);
    }
    uint32_t absoluteAddress = maskRegion(address);
    optional<uint32_t> offset;
    offset = ramRange.contains(absoluteAddress);
    if (offset) {
        ram->storeHalfWord(*offset, value);
        return;
    }
    offset = soundProcessingUnitRange.contains(absoluteAddress);
    if (offset) {
        cout << "Unhandled Sound Processing Unit write at offset: 0x" << hex << *offset << endl;
        return;
    }
    offset = timerRegisterRange.contains(absoluteAddress);
    if (offset) {
        cout << "Unhandled Timer Register write at offset: 0x" << hex << *offset << endl;
        return;
    }
    offset = interruptRequestControlRange.contains(absoluteAddress);
    if (offset) {
        cout << "Unhandled Interrupt Request Control write at offset: 0x" << hex << *offset << endl;
        return;
    }
    cout << "Unhandled half write at: 0x" << hex << address << endl;
    exit(1);
}

void Interconnect::storeByte(uint32_t address, uint8_t value) const {
    uint32_t absoluteAddress = maskRegion(address);
    optional<uint32_t> offset;
    offset = expansion2Range.contains(absoluteAddress);
    if (offset) {
        cout << "Unhandled Expansion 2 write at offset: 0x" << hex << *offset << endl;
        return;
    }
    offset = ramRange.contains(absoluteAddress);
    if (offset) {
        return ram->storeByte(*offset, value);
    }
    cout << "Unhandled byte write at: 0x" << hex << address << endl;
    exit(1);
}

uint32_t Interconnect::dmaRegister(uint32_t offset) const {
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
            Channel channel = dma->channelForPort(port);
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
                    cout << "Unhandled DMA access at offset: 0x" << hex << offset << endl;
                    exit(1);
                }
            }
        }
        case 7: {
            switch (lower) {
                case 0: {
                    return dma->ctrlRegister();
                }
                case 4: {
                    return dma->interruptRegister();
                }
                default: {
                    cout << "Unhandled DMA access at offset: 0x" << hex << offset << endl;
                    exit(1);
                }
            }
        }
        default: {
            cout << "Unhandled DMA access at offset: 0x" << hex << offset << endl;
            exit(1);
        }
    }
}

void Interconnect::setDMARegister(uint32_t offset, uint32_t value) const {
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
            Channel& channel = dma->channelForPort(port);
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
                    cout << "Unhandled DMA write access at offset: 0x" << hex << offset << endl;
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
                    dma->setControlRegister(value);
                    break;
                }
                case 4: {
                    dma->setInterruptRegister(value);
                    break;
                }
                default: {
                    cout << "Unhandled DMA write access at offset: 0x" << hex << offset << endl;
                    exit(1);
                }
            }
            break;
        }
        default: {
            cout << "Unhandled DMA write access at offset: 0x" << hex << offset << endl;
            exit(1);
        }
    }
    if (activePort != Port::None) {
        dma->execute(activePort);
    }
}
