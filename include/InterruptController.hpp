#pragma once
#include <cstdint>

class InterruptController {
    uint16_t status;
    uint16_t mask;

    void setStatus(uint16_t status);
    void setMask(uint16_t mask);
public:
    InterruptController();
    ~InterruptController();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);
};
