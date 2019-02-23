#pragma once
#include <cstdint>

class DMA {
    uint32_t controlRegister;
public:
    DMA();
    ~DMA();

    uint32_t ctrlRegister() const;
    void setControlRegister(uint32_t value);
};
