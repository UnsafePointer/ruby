#pragma once
#include <cstdint>
#include <optional>

class Range {
    const uint32_t start;
    const uint32_t length;
public:
    Range(uint32_t start, uint32_t length);
    ~Range();

    std::optional<uint32_t> contains(uint32_t address) const;
};
