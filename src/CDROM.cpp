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
