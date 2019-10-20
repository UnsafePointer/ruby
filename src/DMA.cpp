#include "DMA.hpp"
#include "RAM.tcc"
#include <iostream>

using namespace std;

DMA::DMA(LogLevel logLevel, unique_ptr<RAM> &ram, unique_ptr<GPU> &gpu, unique_ptr<CDROM> &cdrom, std::unique_ptr<InterruptController> &interruptController) : logger(logLevel, "  DMA: "), ram(ram), gpu(gpu), cdrom(cdrom), interruptController(interruptController) {
    for (int i = 0; i < 7; i++) {
        channels[i] = Channel(logLevel, DMAPort(i));
    }
}

DMA::~DMA() {

}

uint32_t DMA::controlRegister() const {
    logger.logMessage("DPCR [R]: %#x", control.value);
    return control.value;
}

void DMA::setControlRegister(uint32_t value) {
    logger.logMessage("DPCR [W]: %#x", value);
    control.value = value;
}

void DMA::triggerInterrupt(DMAPort port) {
    if (!interrupt.IRQEnableStatus().isPortEnabled(port)) {
        return;
    }
    IRQFlags flags = interrupt.IRQFlagsStatus();
    flags.setFlag(port);
    interrupt._IRQFlags = flags.value;
    uint8_t interruptValue =  calculateInterruptRegister() >> 31 & 1;
    if (interruptValue) {
        shouldTriggerInterrupt = true;
    }
}

void DMA::step() {
    if (shouldTriggerInterrupt) {
        shouldTriggerInterrupt = false;
        interruptController->trigger(InterruptRequestNumber::DMAIRQ);
    }
}

uint32_t DMA::calculateInterruptRegister() const {
    /*
    Bit31 is a simple readonly flag that follows the following rules:
    IF b15=1 OR (b23=1 AND (b16-22 AND b24-30)>0) THEN b31=1 ELSE b31=0
    */
    uint32_t value = interrupt.value;
    uint32_t interrupts = interrupt.IRQEnableStatus().value & interrupt.IRQFlagsStatus().value;
    if (interrupt.forceIRQ || (interrupt.masterIRQEnable && interrupts > 0)) {
        value |= (1UL << 31);
    }
    return value;
}

uint32_t DMA::interruptRegister() const {
    logger.logMessage("DICR [R]: %#x", interrupt.value);
    uint32_t value = calculateInterruptRegister();
    return value;
}

void DMA::setInterruptRegister(uint32_t value) {
    logger.logMessage("DICR [W]: %#x", value);
    value &= ~(1UL << 6);
    value &= ~(1UL << 7);
    value &= ~(1UL << 8);
    value &= ~(1UL << 9);
    value &= ~(1UL << 10);
    value &= ~(1UL << 11);
    value &= ~(1UL << 12);
    value &= ~(1UL << 13);
    value &= ~(1UL << 14);
    value &= ~(1UL << 31);
    DMAInterrupt toWrite = DMAInterrupt();
    toWrite.value = value;
    IRQFlags flags = interrupt.IRQFlagsStatus();
    flags.acknowledge(toWrite.IRQFlagsStatus());
    toWrite._IRQFlags = flags.value;
    interrupt.value = toWrite.value;
}

Channel& DMA::channelForPort(DMAPort port) {
    return channels[port];
}

void DMA::execute(DMAPort port) {
    Channel& channel = channels[port];
    if (channel.sync() == Sync::LinkedList) {
        executeLinkedList(port, channel);
    } else {
        executeBlock(port, channel);
    }
    triggerInterrupt(port);
    return;
}

void DMA::executeLinkedList(DMAPort port, Channel& channel) {
    uint32_t address = channel.baseAddressRegister() & 0x1ffffc;
    if (port != DMAPort::GPUP) {
        logger.logError("Unhandled DMA linked-list transfer with port: %s", portDescription(port).c_str());
    }
    if (channel.direction() == Direction::ToRam) {
        logger.logError("Unhandled DMA linked-list transfer to RAM");
    }
    logger.logWarning("LinkedList for port: %s with base address: %#x", portDescription(port).c_str(), address);
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

void DMA::executeBlock(DMAPort port, Channel& channel) {
    int8_t step = 4;
    if (channel.step() == Step::Decrement) {
        step *= -1;
    }
    uint32_t address = channel.baseAddressRegister();
    optional<uint32_t> transferSize = channel.transferSize();
    if (!transferSize) {
        logger.logError("Unknown DMA transfer size");
    }
    uint32_t remainingTransferSize = *transferSize;
    logger.logWarning("Block for port: %s with base address: %#x and transfer size: %#x", portDescription(port).c_str(), address, remainingTransferSize);
    while (remainingTransferSize > 0) {
        uint32_t currentAddress = address & 0x1ffffc;
        switch (channel.direction()) {
            case Direction::FromRam: {
                uint32_t source = ram->load<uint32_t>(currentAddress);
                switch (port) {
                    case DMAPort::GPUP: {
                        gpu->executeGp0(source);
                        break;
                    }
                    default: {
                        logger.logError("Unhandled DMA block transfer from RAM to source port: %s", portDescription(port).c_str());
                        break;
                    }
                }
                break;
            }
            case Direction::ToRam: {
                uint32_t source = 0;
                switch (port) {
                    case DMAPort::OTC: {
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
                    case DMAPort::CDROMP: {
                        source = cdrom->loadWordFromReadBuffer();
                        break;
                    }
                    default: {
                        logger.logError("Unhandled DMA block transfer to RAM from source port: %s", portDescription(port).c_str());
                        break;
                    }
                }
                ram->store<uint32_t>(currentAddress, source);
                break;
            }
        }
        address += step;
        remainingTransferSize -= 1;
    }
    channel.done();
    return;
}

DMAPort DMA::portWithIndex(uint32_t index) {
    if (index > DMAPort::OTC) {
        logger.logError("Attempting to get port with out-of-bounds index: %d", index);
    }
    return DMAPort(index);
}

string DMA::portDescription(DMAPort port) {
    switch (port) {
        case MDECin:
            return "MDECin";
        case MDECout:
            return "MDECout";
        case GPUP:
            return "GPU";
        case CDROMP:
            return "CDROM";
        case SPUP:
            return "SPU";
        case PIO:
            return "PIO";
        case OTC:
            return "OTC";
        default:
            return "None";
    }
}
