#pragma once
#include <cstdint>
#include <optional>

enum RangeIdentifier : uint32_t {
    RAMR = 0,
    Expansion1R = 1,
    ScratchpadR = 2,
    MemoryControlR = 3,
    ControllerRegisterR = 4,
    RamSizeR = 5,
    InterruptRequestControlR = 6,
    DMARegisterR = 7,
    Timer0RegisterR = 8,
    Timer1RegisterR = 9,
    Timer2RegisterR = 10,
    CDROMRegisterR = 11,
    GPURegisterR = 12,
    SoundProcessingUnitR = 13,
    Expansion2R = 14,
    BIOSR = 15,
    CacheControlR = 16,
};

class Range {
    const uint32_t start;
    const uint32_t length;
public:
    const RangeIdentifier identifier;
    Range(RangeIdentifier identifier, uint32_t start, uint32_t length);
    ~Range();

    int32_t contains(uint32_t address) const;
    uint32_t offset(uint32_t address) const;
};
