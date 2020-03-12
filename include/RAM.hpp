#pragma once
#include <cstdint>
#include <string>
#include <filesystem>

const uint32_t RAM_SIZE = 2*1024*1024;

class RAM {
    uint8_t data[RAM_SIZE];
public:
    RAM();
    ~RAM();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);

    void receiveTransfer(std::filesystem::path filePath, uint32_t origin, uint32_t size, uint32_t destination);
    void dump();
};
