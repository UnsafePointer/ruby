#include "GPU.hpp"
#include <iostream>

using namespace std;

uint8_t horizontalResolutionFromValues(uint8_t value1, uint8_t value2) {
    return ((value2 & 1) | (value1 & 3 << 1));
}

TexturePageColors texturePageColorsWithValue(uint32_t value) {
    if (value > TexturePageColors::T15Bit) {
        cout << "Attempting to create Texture Page Colors out-of-bounds value: " << dec << value << endl;
        exit(1);
    }
    return TexturePageColors(value);
}

GPU::GPU() : texturePageBaseX(0),
             texturePageBaseY(0),
             semiTransparency(0),
             texturePageColors(TexturePageColors::T4Bit),
             ditheringEnable(false),
             allowDrawToDisplayArea(false),
             shouldSetMaskBit(false),
             shouldPreserveMaskedPixels(false),
             interlaceField(Field::Top),
             textureDisable(false),
             horizontalResolution(horizontalResolutionFromValues(0, 0)),
             verticalResolution(VerticalResolution::Y240),
             videoMode(VideoMode::NTSC),
             displayAreaColorDepth(DisplayAreaColorDepth::D15Bits),
             verticalInterlaceEnable(false),
             displayDisable(true),
             interruptRequestEnable(false),
             dmaDirection(GPUDMADirection::Off),
             rectangleTextureFlipX(false),
             rectangleTextureFlipY(false),
             textureWindowMaskX(0),
             textureWindowMaskY(0),
             textureWindowOffsetX(0),
             textureWindowOffsetY(0),
             drawingAreaTop(0),
             drawingAreaLeft(0),
             drawingAreaBottom(0),
             drawingAreaRight(0),
             drawingOffsetX(0),
             drawingOffsetY(0),
             displayVRAMStartX(0),
             displayVRAMStartY(0),
             displayHorizontalStart(0),
             displayHorizontalEnd(0),
             displayLineStart(0),
             displayLineEnd(0)
{
}

GPU::~GPU() {
}

uint32_t GPU::statusRegister() const {
    uint32_t value = 0;
    value |= ((uint32_t)texturePageBaseX) << 0;
    value |= ((uint32_t)texturePageBaseY) << 4;
    value |= ((uint32_t)semiTransparency) << 5;
    value |= ((uint32_t)texturePageColors) << 7;
    value |= ((uint32_t)ditheringEnable) << 9;
    value |= ((uint32_t)allowDrawToDisplayArea) << 10;
    value |= ((uint32_t)shouldSetMaskBit) << 11;
    value |= ((uint32_t)shouldPreserveMaskedPixels) << 12;
    value |= ((uint32_t)interlaceField) << 13;
    // Nobody knows what bit 14 really does
    value |= ((uint32_t)textureDisable) << 15;
    value |= ((uint32_t)horizontalResolution) << 16;
    value |= ((uint32_t)verticalResolution) << 19;
    value |= ((uint32_t)videoMode) << 20;
    value |= ((uint32_t)displayAreaColorDepth) << 21;
    value |= ((uint32_t)verticalInterlaceEnable) << 22;
    value |= ((uint32_t)displayDisable) << 23;
    value |= ((uint32_t)interruptRequestEnable) << 24;
    value |= ((uint32_t)1) << 26; // Ready to receive command
    value |= ((uint32_t)1) << 27; // Ready to send VRAM to CPU
    value |= ((uint32_t)1) << 28; // Ready to receive DMA
    value |= ((uint32_t)dmaDirection) << 29;
    value |= ((uint32_t)0) << 31; // current drawn line?
    uint32_t dmaRequest;
    switch (dmaDirection) {
        case GPUDMADirection::Off: {
            dmaRequest = 0;
            break;
        }
        case GPUDMADirection::Fifo: {
            dmaRequest = 1;
            break;
        }
        case GPUDMADirection::CPUToGp0: {
            dmaRequest = (value >> 28) & 1;
            break;
        }
        case GPUDMADirection::VRAMToCPU: {
            dmaRequest = (value >> 27) & 1;
            break;
        }
    }
    value |= (dmaRequest << 25);
    return value;
}

void GPU::executeGp0(uint32_t value) {
    uint32_t opCode = (value >> 24) & 0xff;
    switch (opCode) {
        case 0x00: {
            // NOP
            break;
        }
        case 0xe1: {
            operationGp0DrawMode(value);
            break;
        }
        default: {
            cout << "Unhandled gp0 instruction 0x" << hex << opCode << endl;
            exit(1);
        }
    }
}

void GPU::executeGp1(uint32_t value) {
    uint32_t opCode = (value >> 24) & 0xff;
    switch (opCode) {
        case 0x00: {
            operationGp1Reset(value);
            break;
        }
        default: {
            cout << "Unhandled gp1 instruction 0x" << hex << opCode << endl;
            exit(1);
        }
    }
}

/*
GP0(E1h) - Draw Mode setting (aka "Texpage")
0-3   Texture page X Base   (N*64) (ie. in 64-halfword steps)    ;GPUSTAT.0-3
4     Texture page Y Base   (N*256) (ie. 0 or 256)               ;GPUSTAT.4
5-6   Semi Transparency     (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4)   ;GPUSTAT.5-6
7-8   Texture page colors   (0=4bit, 1=8bit, 2=15bit, 3=Reserved);GPUSTAT.7-8
9     Dither 24bit to 15bit (0=Off/strip LSBs, 1=Dither Enabled) ;GPUSTAT.9
10    Drawing to display area (0=Prohibited, 1=Allowed)          ;GPUSTAT.10
11    Texture Disable (0=Normal, 1=Disable if GP1(09h).Bit0=1)   ;GPUSTAT.15
        (Above might be chipselect for (absent) second VRAM chip?)
12    Textured Rectangle X-Flip   (BIOS does set this bit on power-up...?)
13    Textured Rectangle Y-Flip   (BIOS does set it equal to GPUSTAT.13...?)
14-23 Not used (should be 0)
24-31 Command  (E1h)
*/
void GPU::operationGp0DrawMode(uint32_t value) {
    texturePageBaseX = value & 0xf;
    texturePageBaseY = (value >> 4) & 1;
    semiTransparency = (value >> 5) & 3;
    texturePageColors = texturePageColorsWithValue((value >> 7) & 3);
    ditheringEnable = ((value >> 9) & 1) != 0;
    allowDrawToDisplayArea = ((value >> 10) & 1) != 0;
    textureDisable = ((value >> 11) & 1) != 0;
    rectangleTextureFlipX = ((value >> 12) & 1) != 0;
    rectangleTextureFlipY = ((value >> 13) & 1) != 0;
}

/*
GP1(00h) - Reset GPU
0-23  Not used (zero)
Resets the GPU to the following values:
GP1(01h)      ;clear fifo
GP1(02h)      ;ack irq (0)
GP1(03h)      ;display off (1)
GP1(04h)      ;dma off (0)
GP1(05h)      ;display address (0)
GP1(06h)      ;display x1,x2 (x1=200h, x2=200h+256*10)
GP1(07h)      ;display y1,y2 (y1=010h, y2=010h+240)
GP1(08h)      ;display mode 320x200 NTSC (0)
GP0(E1h..E6h) ;rendering attributes (0)
*/
void GPU::operationGp1Reset(uint32_t value) {
    interruptRequestEnable = false;

    texturePageBaseX = 0;
    texturePageBaseY = 0;
    semiTransparency = 0;
    texturePageColors = TexturePageColors::T4Bit;
    textureWindowMaskX = 0;
    textureWindowMaskY = 0;
    textureWindowOffsetX = 0;
    textureWindowOffsetY = 0;
    ditheringEnable = false;
    allowDrawToDisplayArea = false;
    textureDisable = false;
    rectangleTextureFlipX = false;
    rectangleTextureFlipY = false;
    drawingAreaLeft = 0;
    drawingAreaTop = 0;
    drawingAreaRight = 0;
    drawingAreaBottom = 0;
    drawingOffsetX = 0;
    drawingOffsetY = 0;
    shouldSetMaskBit = false;
    shouldPreserveMaskedPixels = false;

    dmaDirection = GPUDMADirection::Off;

    displayDisable = true;
    displayVRAMStartX = 0;
    displayVRAMStartY = 0;
    horizontalResolution = horizontalResolutionFromValues(0, 0);
    verticalResolution = VerticalResolution::Y240;

    videoMode = VideoMode::NTSC;
    verticalInterlaceEnable = false;
    displayHorizontalStart = 0x200;
    displayHorizontalEnd = 0xc00;
    displayLineStart = 0x10;
    displayLineEnd = 0x100;
    displayAreaColorDepth = DisplayAreaColorDepth::D15Bits;

    // TODO: clear the command FIFO
    // TODO: invalidate GPU cache
}

uint32_t GPU::readRegister() const {
    return 0;
}
