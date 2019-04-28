#pragma once
#include <cstdint>

class CDROM {
    uint8_t index;

    void setIndex(uint8_t index);
public:
    CDROM();
    ~CDROM();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);
};
