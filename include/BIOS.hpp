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
    template <typename T>
    inline T load(uint32_t offset) const;
    uint32_t loadWord(uint32_t offset) const;
    uint8_t loadByte(uint32_t offset) const;
};
