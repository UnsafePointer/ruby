#pragma once
#include <cstdint>
#include <string>

const uint32_t BIOS_SIZE = 512*1024;

class BIOS {
    uint8_t data[BIOS_SIZE];
public:
    BIOS();
    ~BIOS();

    void loadBin(const std::string& path);
    uint32_t readWord(uint32_t offset) const;
};
