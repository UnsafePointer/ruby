#include "CDROM.hpp"
#include "Output.hpp"

using namespace std;

CDROM::CDROM() : status(), parameters() {

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
        clearParameters();
    }
}

uint8_t CDROM::getStatusRegister() const {
    return status._value;
}

void CDROM::clearParameters() {
    queue<uint8_t> empty;
    swap(parameters, empty);
}

void CDROM::pushParameter(uint8_t value) {
    if (parameters.size() >= 16) {
        printError("Parameter FIFO full");
    }
    parameters.push(value);
    updateStatusRegister();
}

void CDROM::updateStatusRegister() {
    status.parameterFifoEmpty = parameters.empty();
    status.parameterFifoFull = !(parameters.size() >= 16);
}
