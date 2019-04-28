#include "CDROM.hpp"

CDROM::CDROM() : index() {

}

CDROM::~CDROM() {

}

void CDROM::setIndex(uint8_t index) {
    this->index = index & 0x3;
}
