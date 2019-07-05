#pragma once
#include <string>

const uint32_t EXPANSION1_SIZE = 512*1024;

class Expansion1 {
    uint8_t data[EXPANSION1_SIZE];
public:
    Expansion1();
    ~Expansion1();

    void loadBin(const std::string& path);
    template <typename T>
    inline T load(uint32_t offset) const;
};
