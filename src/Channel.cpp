#include "Channel.hpp"
#include <iostream>

using namespace std;

Channel::Channel() : enable(false), direction(Direction::ToRam), step(Step::Increment), sync(Sync::Manual), trigger(false), chop(false), chopDMAWindowSize(0), chopCPUWindowSize(0), unknown(0), baseAddress(0), blockSize(0), blockCount(0)  {

}

Channel::~Channel() {

}

uint32_t Channel::controlRegister() const {
    uint32_t value = 0;
    value |= ((uint32_t)direction) << 0;
    value |= ((uint32_t)step) << 1;
    value |= ((uint32_t)chop) << 8;
    value |= ((uint32_t)sync) << 9;
    value |= ((uint32_t)chopDMAWindowSize) << 16;
    value |= ((uint32_t)chopCPUWindowSize) << 20;
    value |= ((uint32_t)enable) << 24;
    value |= ((uint32_t)trigger) << 28;
    value |= ((uint32_t)unknown) << 29;
    return value;
}

void Channel::setControlRegister(uint32_t value) {
    if ((value & 1) != 0) {
        direction = Direction::FromRam;
    } else {
        direction = Direction::ToRam;
    }
    if (((value >> 1) & 1) != 0) {
        step = Step::Decrement;
    } else {
        step = Step::Increment;
    }
    chop = ((value >> 8) & 1) != 0;
    switch ((value >> 9) & 3) {
        case 0:
            sync = Sync::Manual;
            break;
        case 1:
            sync = Sync::Request;
            break;
        case 2:
            sync = Sync::LinkedList;
            break;
        default:
            cout << "Unknown DMA sync mode" << endl;
            exit(1);
    }
    chopDMAWindowSize = ((value >> 16) & 7);
    chopCPUWindowSize = ((value >> 20) & 7);
    enable = ((value >> 24) & 1) != 0;
    trigger = ((value >> 28) & 1) != 0;
    unknown = ((value >> 29) & 3) != 0;
}

uint32_t Channel::baseAddressRegister() const {
    return baseAddress;
}

void Channel::setBaseAddressRegister(uint32_t value) {
    baseAddress = (value & 0xffffff);
}

uint32_t Channel::blockControlRegister() const {
    uint32_t blockSizeData = blockSize;
    uint32_t blockCountData = blockCount;

    return (blockCountData << 16) | blockSizeData;
}

void Channel::setBlockControlRegister(uint32_t value) {
    blockSize = value & 0xffff;
    blockCount = (value >> 16) & 0xffff;
}

bool Channel::isActive() const {
    if (sync == Sync::Manual) {
        return enable && trigger;
    }
    return enable;
}

Sync Channel::snc() const {
    return sync;
}
