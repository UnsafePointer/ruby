#pragma once
#include <cstdint>
#include "Logger.hpp"

class SPU {
    Logger logger;
public:
    SPU(LogLevel logLevel);
    ~SPU();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);
};
