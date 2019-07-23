#include "CDROM.hpp"

CDROM::CDROM() : status() {

}

CDROM::~CDROM() {

}

void CDROM::setStatusRegister(uint8_t value) {
    status.index = value & 0x3;
}

void CDROM::setInterruptRegister(uint8_t value) {
    interrupt.enable = value;
}

void CDROM::setInterruptFlagRegister(uint8_t value) {
    if (value & 0x40) {
        status.parameterFifoEmpty = 1;
        status.parameterFifoFull = 1;
        // TODO: Reset parameter FIFO
    }
}

uint8_t CDROM::getStatusRegister() const {
    return status._value;
}
