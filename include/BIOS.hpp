#pragma once
#include <cstdint>
#include <string>
#include <optional>

const uint32_t BIOS_SIZE = 512*1024;

class BIOS {
    uint8_t data[BIOS_SIZE];

    std::optional<std::string> checkAFunctions(uint32_t r9);
    std::optional<std::string> checkBFunctions(uint32_t r9);
    std::optional<std::string> checkCFunctions(uint32_t r9);
public:
    BIOS();
    ~BIOS();

    std::optional<std::string> checkFunctions(uint32_t programCounter, uint32_t r9);

    void loadBin(const std::string& path);
    template <typename T>
    inline T load(uint32_t offset) const;
};
