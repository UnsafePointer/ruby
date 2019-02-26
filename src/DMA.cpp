#include "DMA.hpp"
#include <iostream>

using namespace std;

Port portWithIndex(uint32_t index) {
    if (index > Port::OTC) {
        cout << "Attempting to get port with out-of-bounds index: " << dec << index << endl;
        exit(1);
    }
    return Port(index);
}

DMA::DMA() : controlRegister(0x07654321) {
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
        cout << "Linked List unimplemented" << endl;
        exit(1);
    } else {
        executeBlock(channel);
    }
    return;
}

void DMA::executeBlock(Channel channel) {
    return;
}
