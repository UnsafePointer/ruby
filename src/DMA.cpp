#include "DMA.hpp"
#include <iostream>
#include "RAM.tcc"

using namespace std;

Port portWithIndex(uint32_t index) {
    if (index > Port::OTC) {
        cout << "Attempting to get port with out-of-bounds index: " << dec << index << endl;
        exit(1);
    }
    return Port(index);
}

DMA::DMA(unique_ptr<RAM> &ram, unique_ptr<GPU> &gpu) : ram(ram), gpu(gpu), controlRegister(0x07654321) {
    for (int i = 0; i < 7; i++) {
        channels[i] = Channel();
    }
}

DMA::~DMA() {

}

uint32_t DMA::ctrlRegister() const {
    return controlRegister;
}

void DMA::setControlRegister(uint32_t value) {
    controlRegister = value;
}

bool DMA::interruptRequestStatus() const {
    uint8_t channelStatus = interruptRequestChannelFlags & interruptRequestChannelEnable;
    return forceInterruptRequest || (interruptRequestChannelEnable && channelStatus != 0);
}

uint32_t DMA::interruptRegister() const {
    uint32_t value = 0;
    value |= interruptRequestUnknown;
    value |= ((uint32_t)forceInterruptRequest) << 15;
    value |= ((uint32_t)interruptRequestChannelEnable) << 16;
    value |= ((uint32_t)interruptRequestEnable) << 23;
    value |= ((uint32_t)interruptRequestChannelFlags) << 24;
    value |= ((uint32_t)interruptRequestStatus()) << 31;
    return value;
}

void DMA::setInterruptRegister(uint32_t value) {
    interruptRequestUnknown = (value & 0x3f);
    forceInterruptRequest = (value >> 15) & 1;
    interruptRequestChannelEnable = ((value >> 16) & 0x7f);
    interruptRequestEnable = (value >> 23) & 1;

    uint8_t flagReset = ((value >> 24) & 0x3f);
    interruptRequestChannelFlags &= !flagReset;
}

Channel& DMA::channelForPort(Port port) {
    return channels[port];
}

void DMA::execute(Port port) {
    Channel& channel = channels[port];
    if (channel.snc() == Sync::LinkedList) {
        executeLinkedList(port, channel);
    } else {
        executeBlock(port, channel);
    }
    return;
}

void DMA::executeLinkedList(Port port, Channel& channel) {
    uint32_t address = channel.baseAddressRegister() & 0x1ffffc;
    if (channel.dir() == Direction::ToRam) {
        cout << "Unhandled DMA direction" << endl;
        exit(1);
    }
    if (port != Port::GPUP) {
        cout << "Unhandled DMA port" << endl;
        exit(1);
    }
    while (true) {
        uint32_t header = ram->load<uint32_t>(address);
        uint32_t remainingTransferSize = header >> 24;
        while (remainingTransferSize > 0) {
            address = (address + 4) & 0x1ffffc;
            uint32_t command = ram->load<uint32_t>(address);
            gpu->executeGp0(command);
            remainingTransferSize -= 1;
        }
        if ((header & 0x800000) != 0) {
            break;
        }
        address = header & 0x1ffffc;
    }
    channel.done();
    return;
}

void DMA::executeBlock(Port port, Channel& channel) {
    int8_t step = 4;
    if (channel.stp() == Step::Decrement) {
        step *= -1;
    }
    uint32_t address = channel.baseAddressRegister();
    optional<uint32_t> transferSize = channel.transferSize();
    if (!transferSize) {
        cout << "Unknown DMA transfer size" << endl;
        exit(1);
    }
    uint32_t remainingTransferSize = *transferSize;
    while (remainingTransferSize > 0) {
        uint32_t currentAddress = address & 0x1ffffc;
        switch (channel.dir()) {
            case Direction::FromRam: {
                uint32_t source = ram->load<uint32_t>(currentAddress);
                switch (port) {
                    case Port::GPUP: {
                        gpu->executeGp0(source);
                        break;
                    }
                    default: {
                        cout << "Unhandled DMA source port" << endl;
                        exit(1);
                        break;
                    }
                }
                break;
            }
            case Direction::ToRam: {
                uint32_t source;
                switch (port) {
                    case Port::OTC: {
                        switch (remainingTransferSize) {
                            case 1: {
                                source = 0xffffff;
                                break;
                            }
                            default: {
                                source = (address - 4) & 0x1fffff;
                                break;
                            }
                        }
                        break;
                    }
                    default: {
                        cout << "Unhandled DMA source port" << endl;
                        exit(1);
                        break;
                    }
                }
                ram->storeWord(currentAddress, source);
                break;
            }
        }
        address += step;
        remainingTransferSize -= 1;
    }
    channel.done();
    return;
}
