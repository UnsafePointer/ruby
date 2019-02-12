#pragma once
#include <cstdint>

struct RegisterIndex {
    RegisterIndex(uint8_t index);
    ~RegisterIndex();
    uint8_t idx();
private:
    uint8_t index;
};
