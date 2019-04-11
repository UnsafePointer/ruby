#pragma once
#include <cstdint>

const uint32_t VRAM_SIZE = (1024 * 512) / sizeof(uint16_t);

class GPUImageBuffer {
    uint16_t destinationX;
    uint16_t destinationY;
    uint16_t width;
    uint16_t heigth;
    uint32_t index;
    uint16_t buffer[VRAM_SIZE];
public:
    GPUImageBuffer();
    ~GPUImageBuffer();

    void reset(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void pushWord(uint32_t word);
    bool isValid();
};
