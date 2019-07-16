#pragma once
#include <cstdint>
#include <tuple>

const uint32_t VRAM_WIDTH = 1024;
const uint32_t VRAM_HEIGHT = 512;
const uint32_t VRAM_SIZE = (VRAM_WIDTH * VRAM_HEIGHT) / sizeof(uint16_t);

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

    std::pair<uint16_t, uint16_t> destination();
    std::pair<uint16_t, uint16_t> resolution();
    void reset(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void pushWord(uint32_t word);
    bool isValid();
    uint16_t* bufferRef();
};
