#include "SPU.hpp"

SPU::SPU(LogLevel logLevel) : logger(logLevel, "  SPU: "), control() {

}

SPU::~SPU() {

}

uint16_t SPU::controlRegister() const {
    return control.value;
}

void SPU::setControlRegister(uint16_t value) {
    control.value = value;
}
