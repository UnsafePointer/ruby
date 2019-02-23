#include "DMA.hpp"

DMA::DMA() : controlRegister(0x07654321) {

}

DMA::~DMA() {

}

uint32_t DMA::ctrlRegister() const {
    return controlRegister;
}

void DMA::setControlRegister(uint32_t value) {
    controlRegister = value;
}
