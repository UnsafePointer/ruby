#pragma once
#include "BIOS.hpp"

class Interconnect {
    const BIOS &bios;
public:
    Interconnect(BIOS &bios);
    ~Interconnect();

    uint32_t readWord(uint32_t address) const;
    void storeWord(uint32_t address, uint32_t value) const;
};
