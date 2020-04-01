#pragma once
#include <cstdint>
#include "Logger.hpp"

class GTE {
    Logger logger;
public:
    GTE(LogLevel logLevel);
    ~GTE();

    void execute(uint32_t value);
};
