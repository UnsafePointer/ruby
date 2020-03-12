#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <array>
#include <filesystem>
#include "Logger.hpp"

const uint32_t BIOS_SIZE = 512*1024;

class BIOS {
    uint8_t data[BIOS_SIZE];
    Logger logger;

    std::string formatBIOSFunction(std::string function, unsigned int argc, std::array<uint32_t, 4> subroutineArguments);

    std::optional<std::string> checkAFunctions(uint32_t r9, std::array<uint32_t, 4> subroutineArguments);
    std::optional<std::string> checkBFunctions(uint32_t r9, std::array<uint32_t, 4> subroutineArguments);
    std::optional<std::string> checkCFunctions(uint32_t r9, std::array<uint32_t, 4> subroutineArguments);
public:
    BIOS(LogLevel logLevel);
    ~BIOS();

    std::optional<std::string> checkFunctions(uint32_t programCounter, uint32_t r9, std::array<uint32_t, 4> subroutineArguments);

    void loadBin(const std::filesystem::path& filePath);
    template <typename T>
    inline T load(uint32_t offset) const;
};
