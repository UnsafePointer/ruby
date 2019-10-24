#pragma once
#include <cstdint>

enum DMAPort : uint8_t {
    MDECin = 0,
    MDECout = 1,
    GPUP = 2,
    CDROMP = 3,
    SPUP = 4,
    PIO = 5,
    OTC = 6,
    None = 7
};
