#include "GPU.hpp"
#include "Output.hpp"
#include "Vertex.hpp"

using namespace std;

uint8_t horizontalResolutionFromValues(uint8_t value1, uint8_t value2) {
    return ((value2 & 1) | (value1 & 3 << 1));
}

TexturePageColors texturePageColorsWithValue(uint32_t value) {
    if (value > TexturePageColors::T15Bit) {
        printError("Attempting to create Texture Page Colors out-of-bounds value: %#x", value);
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
             displayLineEnd(0),
             gp0InstructionBuffer(GPUInstructionBuffer()),
             gp0WordsRemaining(0),
             gp0InstructionMethod(nullptr),
             gp0Mode(GP0Mode::Command),
             renderer(Renderer()),
             imageBuffer(make_unique<GPUImageBuffer>())
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
    // TODO: remove this commented out line
    // value |= ((uint32_t)verticalResolution) << 19;
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
    if (gp0WordsRemaining == 0) {
        uint32_t opCode = (value >> 24) & 0xff;
        switch (opCode) {
            case 0x00: {
                gp0WordsRemaining = 1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0Nop();
                };
                break;
            }
            case 0x01: {
                gp0WordsRemaining = 1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0ClearCache();
                };
                break;
            }
            case 0x02: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0FillRectagleInVRAM();
                };
                break;
            }
            case 0x28: {
                gp0WordsRemaining = 5;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeQuadOpaque();
                };
                break;
            }
            case 0x2c: {
                gp0WordsRemaining = 9;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedFourPointOpaqueTextureBlending();
                };
                break;
            }
            case 0x30: {
                gp0WordsRemaining = 6;
                gp0InstructionMethod = [&]() {
                    this->operationGp0ShadedTriangleOpaque();
                };
                break;
            }
            case 0x38: {
                gp0WordsRemaining = 8;
                gp0InstructionMethod = [&]() {
                    this->operationGp0ShadedQuadOpaque();
                };
                break;
            }
            case 0x64: {
                gp0WordsRemaining = 4;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuadOpaqueTextureBlending();
                };
                break;
            }
            case 0x65: {
                gp0WordsRemaining = 4;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuadOpaqueRawTexture();
                };
                break;
            }
            case 0x66: {
                gp0WordsRemaining = 4;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuadOpaqueRawTexture();
                };
                break;
            }
            case 0x67: {
                gp0WordsRemaining = 4;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuadOpaqueRawTexture();
                };
                break;
            }
            case 0x68: {
                gp0WordsRemaining = 2;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeRectangle1x1DotOpaque();
                };
                break;
            }
            case 0x6c: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuad1x1OpaqueTextureBlending();
                };
                break;
            }
            case 0x6d: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuad1x1OpaqueRawTexture();
                };
                break;
            }
            case 0x6e: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuad1x1SemiTransparentTextureBlending();
                };
                break;
            }
            case 0x6f: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuad1x1SemiTransparentRawTexture();
                };
                break;
            }
            case 0x74: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuad8x8OpaqueTextureBlending();
                };
                break;
            }
            case 0x75: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuad8x8OpaqueRawTexture();
                };
                break;
            }
            case 0x76: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuad8x8SemiTransparentTextureBlending();
                };
                break;
            }
            case 0x77: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuad8x8SemiTransparentRawTexture();
                };
                break;
            }
            case 0x7c: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuad16x16OpaqueTextureBlending();
                };
                break;
            }
            case 0x7d: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuad16x16OpaqueRawTexture();
                };
                break;
            }
            case 0x7e: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuad16x16SemiTransparentTextureBlending();
                };
                break;
            }
            case 0x7f: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedQuad16x16SemiTransparentRawTexture();
                };
                break;
            }
            case 0xa0: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0CopyRectangleCPUToVRAM();
                };
                break;
            }
            case 0xc0: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0CopyRectangleVRAMToCPU();
                };
                break;
            }
            case 0xe1: {
                gp0WordsRemaining = 1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0DrawMode();
                };
                break;
            }
            case 0xe2: {
                gp0WordsRemaining = 1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TextureWindowSetting();
                };
                break;
            }
            case 0xe3: {
                gp0WordsRemaining = 1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0SetDrawingAreaTopLeft();
                };
                break;
            }
            case 0xe4: {
                gp0WordsRemaining = 1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0SetDrawingAreaBottomRight();
                };
                break;
            }
            case 0xe5: {
                gp0WordsRemaining = 1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0SetDrawingOffset();
                };
                break;
            }
            case 0xe6: {
                gp0WordsRemaining = 1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MaskBitSetting();
                };
                break;
            }
            default: {
                printError("Unhandled gp0 instruction %#x", opCode);
            }
        }
        gp0InstructionBuffer.clear();
    }
    gp0WordsRemaining -= 1;

    if (gp0Mode == GP0Mode::Command) {
        gp0InstructionBuffer.pushWord(value);
        if (gp0WordsRemaining == 0) {
            gp0InstructionMethod();
        }
    } else if (gp0Mode == GP0Mode::ImageLoad) {
        imageBuffer->pushWord(value);
        if (gp0WordsRemaining == 0) {
            renderer.loadImage(imageBuffer);
            gp0Mode = GP0Mode::Command;
        }
    }
}

void GPU::render() {
    renderer.display();
}

void GPU::executeGp1(uint32_t value) {
    uint32_t opCode = (value >> 24) & 0xff;
    switch (opCode) {
        case 0x00: {
            operationGp1Reset(value);
            break;
        }
        case 0x01: {
            operationGp1ResetCommandBuffer(value);
            break;
        }
        case 0x02: {
            operationGp1AcknowledgeGPUInterrupt(value);
            break;
        }
        case 0x03: {
            operationGp1DisplayEnable(value);
            break;
        }
        case 0x04: {
            operationGp1DMADirection(value);
            break;
        }
        case 0x05: {
            operationGp1StartOfDisplayArea(value);
            break;
        }
        case 0x06: {
            operationGp1HorizontalDisplayRange(value);
            break;
        }
        case 0x07: {
            operationGp1VerticalDisplayRange(value);
            break;
        }
        case 0x08: {
            operationGp1DisplayMode(value);
            break;
        }
        case 0x10: {
            operationGp1GetGPUInfo(value);
            break;
        }
        default: {
            printError("Unhandled gp1 instruction %#x", opCode);
        }
    }
}

void GPU::operationGp0Nop() {
    return;
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
void GPU::operationGp0DrawMode() {
    uint32_t value = gp0InstructionBuffer[0];
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
    return gpuRead;
}

/*
GP1(08h) - Display mode
0-1   Horizontal Resolution 1     (0=256, 1=320, 2=512, 3=640) ;GPUSTAT.17-18
2     Vertical Resolution         (0=240, 1=480, when Bit5=1)  ;GPUSTAT.19
3     Video Mode                  (0=NTSC/60Hz, 1=PAL/50Hz)    ;GPUSTAT.20
4     Display Area Color Depth    (0=15bit, 1=24bit)           ;GPUSTAT.21
5     Vertical Interlace          (0=Off, 1=On)                ;GPUSTAT.22
6     Horizontal Resolution 2     (0=256/320/512/640, 1=368)   ;GPUSTAT.16
7     "Reverseflag"               (0=Normal, 1=Distorted)      ;GPUSTAT.14
8-23  Not used (zero)
*/
void GPU::operationGp1DisplayMode(uint32_t value) {
    uint8_t horizontalResolutionValue1 = (value & 3);
    uint8_t horizontalResolutionValue2 = (value >> 6) & 1;
    horizontalResolution = horizontalResolutionFromValues(horizontalResolutionValue1, horizontalResolutionValue2);
    verticalResolution = VerticalResolution((value >> 2) & 0x1);
    videoMode = VideoMode((value >> 3) & 0x1);
    displayAreaColorDepth = DisplayAreaColorDepth((value >> 4) & 0x1);
    verticalInterlaceEnable = (value >> 5) & 0x1;
    if ((value & 0x80) != 0) {
        // This is supposed to be bit 14 on GPUSTAT
        printError("Unsupported display mode: distorted");
    }
}

/*
GP1(04h) - DMA Direction / Data Request
0-1  DMA Direction (0=Off, 1=FIFO, 2=CPUtoGP0, 3=GPUREADtoCPU) ;GPUSTAT.29-30
2-23 Not used (zero)
*/
void GPU::operationGp1DMADirection(uint32_t value) {
    dmaDirection = GPUDMADirection(value & 3);
}

/*
GP0(E3h) - Set Drawing Area top left (X1,Y1)
0-9    X-coordinate (0..1023)
10-18  Y-coordinate (0..511)   ;\on Old 160pin GPU (max 1MB VRAM)
19-23  Not used (zero)         ;/
10-19  Y-coordinate (0..1023)  ;\on New 208pin GPU (max 2MB VRAM)
20-23  Not used (zero)         ;/(retail consoles have only 1MB though)
24-31  Command  (Exh)
*/
void GPU::operationGp0SetDrawingAreaTopLeft() {
    uint32_t value = gp0InstructionBuffer[0];
    drawingAreaTop = ((value >> 10) & 0x3ff);
    drawingAreaLeft = (value & 0x3ff);
}

/*
GP0(E4h) - Set Drawing Area bottom right (X2,Y2)
0-9    X-coordinate (0..1023)
10-18  Y-coordinate (0..511)   ;\on Old 160pin GPU (max 1MB VRAM)
19-23  Not used (zero)         ;/
10-19  Y-coordinate (0..1023)  ;\on New 208pin GPU (max 2MB VRAM)
20-23  Not used (zero)         ;/(retail consoles have only 1MB though)
24-31  Command  (Exh)
*/
void GPU::operationGp0SetDrawingAreaBottomRight() {
    uint32_t value = gp0InstructionBuffer[0];
    drawingAreaBottom = ((value >> 10) & 0x3ff);
    drawingAreaRight = (value & 0x3ff);
}

/*
GP0(E5h) - Set Drawing Offset (X,Y)
0-10   X-offset (-1024..+1023) (usually within X1,X2 of Drawing Area)
11-21  Y-offset (-1024..+1023) (usually within Y1,Y2 of Drawing Area)
22-23  Not used (zero)
24-31  Command  (E5h)
*/
void GPU::operationGp0SetDrawingOffset() {
    uint32_t value = gp0InstructionBuffer[0];
    uint16_t x = (value & 0x7ff);
    uint16_t y = ((value >> 11) & 0x7ff);

    int16_t drawingOffsetX = ((int16_t)(x << 5)) >> 5;
    int16_t drawingOffsetY = ((int16_t)(y << 5)) >> 5;

    renderer.setDrawingOffset(drawingOffsetX, drawingOffsetY);
    renderer.display();
}

/*
GP0(E2h) - Texture Window setting
0-4    Texture window Mask X   (in 8 pixel steps)
5-9    Texture window Mask Y   (in 8 pixel steps)
10-14  Texture window Offset X (in 8 pixel steps)
15-19  Texture window Offset Y (in 8 pixel steps)
20-23  Not used (zero)
24-31  Command  (E2h)
*/
void GPU::operationGp0TextureWindowSetting() {
    uint32_t value = gp0InstructionBuffer[0];
    textureWindowMaskX = (value & 0x1f);
    textureWindowMaskY = ((value >> 5) & 0x1f);
    textureWindowOffsetX = ((value >> 10) & 0x1f);
    textureWindowOffsetY = ((value >> 15) & 0x1f);
}

/*
GP0(E6h) - Mask Bit Setting
0     Set mask while drawing (0=TextureBit15, 1=ForceBit15=1)   ;GPUSTAT.11
1     Check mask before draw (0=Draw Always, 1=Draw if Bit15=0) ;GPUSTAT.12
2-23  Not used (zero)
24-31 Command  (E6h)
*/
void GPU::operationGp0MaskBitSetting() {
    uint32_t value = gp0InstructionBuffer[0];
    shouldSetMaskBit = (value & 1) != 0;
    shouldPreserveMaskedPixels = (value & 2) != 0;
}

/*
GP1(05h) - Start of Display area (in VRAM)
0-9   X (0-1023)    (halfword address in VRAM)  (relative to begin of VRAM)
10-18 Y (0-511)     (scanline number in VRAM)   (relative to begin of VRAM)
19-23 Not used (zero)
*/
void GPU::operationGp1StartOfDisplayArea(uint32_t value) {
    displayVRAMStartX = (value & 0x3fe);
    displayVRAMStartY = ((value >> 10) & 0x1ff);
}

/*
GP1(06h) - Horizontal Display range (on Screen)
0-11   X1 (260h+0)       ;12bit       ;\counted in 53.222400MHz units,
12-23  X2 (260h+320*8)   ;12bit       ;/relative to HSYNC
*/
void GPU::operationGp1HorizontalDisplayRange(uint32_t value) {
    displayHorizontalStart = (value & 0xfff);
    displayHorizontalEnd = ((value >> 12) & 0xfff);
}

/*
GP1(07h) - Vertical Display range (on Screen)
0-9   Y1 (NTSC=88h-(224/2), (PAL=A3h-(264/2))  ;\scanline numbers on screen,
10-19 Y2 (NTSC=88h+(224/2), (PAL=A3h+(264/2))  ;/relative to VSYNC
20-23 Not used (zero)
*/
void GPU::operationGp1VerticalDisplayRange(uint32_t value) {
    displayLineStart = (value & 0x3ff);
    displayLineEnd = ((value >> 10) & 0x3ff);
}

/*
GP0(28h) - Monochrome four-point polygon, opaque
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex1           (YyyyXxxxh)
3rd  Vertex2           (YyyyXxxxh)
4th  Vertex3           (YyyyXxxxh)
(5th) Vertex4           (YyyyXxxxh) (if any)
*/
void GPU::operationGp0MonochromeQuadOpaque() {
    Color color = Color(gp0InstructionBuffer[0]);
    Point point1 = Point(gp0InstructionBuffer[1]);
    Point point2 = Point(gp0InstructionBuffer[2]);
    Point point3 = Point(gp0InstructionBuffer[3]);
    Point point4 = Point(gp0InstructionBuffer[4]);
    array<Vertex, 4> vertices = {
        Vertex(point1, color),
        Vertex(point2, color),
        Vertex(point3, color),
        Vertex(point4, color),
    };
    renderer.pushQuad(vertices);
    return;
}

void GPU::operationGp0ClearCache() {
    return;
}

/*
GP0(A0h) - Copy Rectangle (CPU to VRAM)
1st  Command           (Cc000000h)
2nd  Destination Coord (YyyyXxxxh)  ;Xpos counted in halfwords
3rd  Width+Height      (YsizXsizh)  ;Xsiz counted in halfwords
...  Data              (...)      <--- usually transferred via DMA
*/
void GPU::operationGp0CopyRectangleCPUToVRAM() {
    Point point = Point(gp0InstructionBuffer[1]);
    Dimensions dimensions = Dimensions(gp0InstructionBuffer[2]);
    uint32_t imageSize = dimensions.width * dimensions.height;
    // Pixels are 16 bit wide and transactions are 32 bit wide
    // If the resolution is odd, add a unit to get the right
    // number of transactions
    if (imageSize % 2 != 0) {
        imageSize++;
    }
    gp0WordsRemaining = imageSize / 2;
    gp0Mode = GP0Mode::ImageLoad;
    imageBuffer->reset(point.x, point.y, dimensions.width, dimensions.height);
    return;
}

/*
GP1(03h) - Display Enable
0     Display On/Off   (0=On, 1=Off)                         ;GPUSTAT.23
1-23  Not used (zero)
*/
void GPU::operationGp1DisplayEnable(uint32_t value) {
    displayDisable = (value & 1) != 0;
}

/*
GP0(C0h) - Copy Rectangle (VRAM to CPU)
1st  Command           (Cc000000h) ;\
2nd  Source Coord      (YyyyXxxxh) ; write to GP0 port (as usually)
3rd  Width+Height      (YsizXsizh) ;/
...  Data              (...)       ;<--- read from GPUREAD port (or via DMA)
*/
void GPU::operationGp0CopyRectangleVRAMToCPU() {
    uint32_t resolution = gp0InstructionBuffer[2];
    uint32_t width = resolution & 0xffff;
    uint32_t height = resolution >> 16;

    printWarning("Unhandled GP0 Copy Rectangle VRAM to CPU with with resolution: %d x %d", width, height);
}

/*
GP0(38h) - Shaded four-point polygon, opaque
1st  Color1+Command    (CcBbGgRrh)
2nd  Vertex1           (YyyyXxxxh)
3rd  Color2            (00BbGgRrh)
4th  Vertex2           (YyyyXxxxh)
5th  Color3            (00BbGgRrh)
6th  Vertex3           (YyyyXxxxh)
(7th) Color4            (00BbGgRrh) (if any)
(8th) Vertex4           (YyyyXxxxh) (if any)
*/
void GPU::operationGp0ShadedQuadOpaque() {
    Color color1 = Color(gp0InstructionBuffer[0]);
    Color color2 = Color(gp0InstructionBuffer[2]);
    Color color3 = Color(gp0InstructionBuffer[4]);
    Color color4 = Color(gp0InstructionBuffer[6]);
    Point point1 = Point(gp0InstructionBuffer[1]);
    Point point2 = Point(gp0InstructionBuffer[3]);
    Point point3 = Point(gp0InstructionBuffer[5]);
    Point point4 = Point(gp0InstructionBuffer[7]);
    array<Vertex, 4> vertices = {
        Vertex(point1, color1),
        Vertex(point2, color2),
        Vertex(point3, color3),
        Vertex(point4, color4),
    };
    renderer.pushQuad(vertices);
    return;
}

/*
GP0(30h) - Shaded three-point polygon, opaque
  1st  Color1+Command    (CcBbGgRrh)
  2nd  Vertex1           (YyyyXxxxh)
  3rd  Color2            (00BbGgRrh)
  4th  Vertex2           (YyyyXxxxh)
  5th  Color3            (00BbGgRrh)
  6th  Vertex3           (YyyyXxxxh)
*/
void GPU::operationGp0ShadedTriangleOpaque() {
    Color color1 = Color(gp0InstructionBuffer[0]);
    Color color2 = Color(gp0InstructionBuffer[2]);
    Color color3 = Color(gp0InstructionBuffer[4]);
    Point point1 = Point(gp0InstructionBuffer[1]);
    Point point2 = Point(gp0InstructionBuffer[3]);
    Point point3 = Point(gp0InstructionBuffer[5]);
    array<Vertex, 3> vertices = {
        Vertex(point1, color1),
        Vertex(point2, color2),
        Vertex(point3, color3),
    };
    renderer.pushTriangle(vertices);
    return;
}

/*
GP0(2Ch) - Textured four-point polygon, opaque, texture-blending
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex1           (YyyyXxxxh)
3rd  Texcoord1+Palette (ClutYyXxh)
4th  Vertex2           (YyyyXxxxh)
5th  Texcoord2+Texpage (PageYyXxh)
6th  Vertex3           (YyyyXxxxh)
7th  Texcoord3         (0000YyXxh)
(8th) Vertex4           (YyyyXxxxh) (if any)
(9th) Texcoord4         (0000YyXxh) (if any)
*/
void GPU::operationGp0TexturedFourPointOpaqueTextureBlending() {
    Color color = Color(gp0InstructionBuffer[0]);
    Point point1 = Point(gp0InstructionBuffer[1]);
    Point point2 = Point(gp0InstructionBuffer[3]);
    Point point3 = Point(gp0InstructionBuffer[5]);
    Point point4 = Point(gp0InstructionBuffer[7]);
    Point texturePoint1 = Point::forTexturePosition(gp0InstructionBuffer[2] & 0xffff);
    Point clut = Point::forClut(gp0InstructionBuffer[2] >> 16);
    Point texturePoint2 = Point::forTexturePosition(gp0InstructionBuffer[4] & 0xffff);
    Point texturePage = Point::forTexturePage(gp0InstructionBuffer[4] >> 16);
    Point texturePoint3 = Point::forTexturePosition(gp0InstructionBuffer[6] & 0xffff);
    Point texturePoint4 = Point::forTexturePosition(gp0InstructionBuffer[8] & 0xffff);
    TexturePageColors texturePageColors = texturePageColorsWithValue(((gp0InstructionBuffer[4] >> 16) >> 7) & 0x3);
    GLuint textureDepthShift = 2 - texturePageColors;
    array<Vertex, 4> vertices = {
        Vertex(point1, color, texturePoint1, TextureBlendModeTextureBlend, texturePage, textureDepthShift, clut),
        Vertex(point2, color, texturePoint2, TextureBlendModeTextureBlend, texturePage, textureDepthShift, clut),
        Vertex(point3, color, texturePoint3, TextureBlendModeTextureBlend, texturePage, textureDepthShift, clut),
        Vertex(point4, color, texturePoint4, TextureBlendModeTextureBlend, texturePage, textureDepthShift, clut),
    };
    renderer.pushQuad(vertices);
    return;
}

/*
GP0(64h) - Textured Rectangle, variable size, opaque, texture-blending
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
(4th) Width+Height      (YsizXsizh) (variable size only) (max 1023x511)
*/
void GPU::operationGp0TexturedQuadOpaqueTextureBlending() {
    Dimensions dimensions = Dimensions(gp0InstructionBuffer[3]);
    texturedQuad(dimensions, true, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(65h) - Textured Rectangle, variable size, opaque, raw-texture
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
(4th) Width+Height      (YsizXsizh) (variable size only) (max 1023x511)
*/
void GPU::operationGp0TexturedQuadOpaqueRawTexture() {
    Dimensions dimensions = Dimensions(gp0InstructionBuffer[3]);
    texturedQuad(dimensions, true, TextureBlendModeRawTexture);
    return;
}

/*
GP0(66h) - Textured Rectangle, variable size, semi-transp, texture-blending
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
(4th) Width+Height      (YsizXsizh) (variable size only) (max 1023x511)
*/
void GPU::operationGp0TexturedSemiTransparentOpaqueTextureBlending() {
    Dimensions dimensions = Dimensions(gp0InstructionBuffer[3]);
    texturedQuad(dimensions, false, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(67h) - Textured Rectangle, variable size, semi-transp, raw-texture
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
(4th) Width+Height      (YsizXsizh) (variable size only) (max 1023x511)
*/
void GPU::operationGp0TexturedSemiTransparentOpaqueRawTexture() {
    Dimensions dimensions = Dimensions(gp0InstructionBuffer[3]);
    texturedQuad(dimensions, false, TextureBlendModeRawTexture);
    return;
}

/*
GP0(6Ch) - Textured Rectangle, 1x1 (nonsense), opaque, texture-blending
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
*/
void GPU::operationGp0TexturedQuad1x1OpaqueTextureBlending() {
    Dimensions dimensions = Dimensions(1, 1);
    texturedQuad(dimensions, true, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(6Dh) - Textured Rectangle, 1x1 (nonsense), opaque, raw-texture
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
*/
void GPU::operationGp0TexturedQuad1x1OpaqueRawTexture() {
    Dimensions dimensions = Dimensions(1, 1);
    texturedQuad(dimensions, true, TextureBlendModeRawTexture);
    return;
}

/*
GP0(6Eh) - Textured Rectangle, 1x1 (nonsense), semi-transp, texture-blending
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
*/
void GPU::operationGp0TexturedQuad1x1SemiTransparentTextureBlending() {
    Dimensions dimensions = Dimensions(1, 1);
    texturedQuad(dimensions, false, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(6Fh) - Textured Rectangle, 1x1 (nonsense), semi-transp, raw-texture
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
*/
void GPU::operationGp0TexturedQuad1x1SemiTransparentRawTexture() {
    Dimensions dimensions = Dimensions(1, 1);
    texturedQuad(dimensions, false, TextureBlendModeRawTexture);
    return;
}

/*
GP0(74h) - Textured Rectangle, 8x8, opaque, texture-blending
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
*/
void GPU::operationGp0TexturedQuad8x8OpaqueTextureBlending() {
    Dimensions dimensions = Dimensions(8, 8);
    texturedQuad(dimensions, true, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(75h) - Textured Rectangle, 8x8, opaque, raw-texture
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
*/
void GPU::operationGp0TexturedQuad8x8OpaqueRawTexture() {
    Dimensions dimensions = Dimensions(8, 8);
    texturedQuad(dimensions, true, TextureBlendModeRawTexture);
    return;
}

/*
GP0(76h) - Textured Rectangle, 8x8, semi-transparent, texture-blending
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
*/
void GPU::operationGp0TexturedQuad8x8SemiTransparentTextureBlending() {
    Dimensions dimensions = Dimensions(8, 8);
    texturedQuad(dimensions, false, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(77h) - Textured Rectangle, 8x8, semi-transparent, raw-texture
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
*/
void GPU::operationGp0TexturedQuad8x8SemiTransparentRawTexture() {
    Dimensions dimensions = Dimensions(8, 8);
    texturedQuad(dimensions, false, TextureBlendModeRawTexture);
    return;
}

/*
GP0(7Ch) - Textured Rectangle, 16x16, opaque, texture-blending
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
*/
void GPU::operationGp0TexturedQuad16x16OpaqueTextureBlending() {
    Dimensions dimensions = Dimensions(16, 16);
    texturedQuad(dimensions, true, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(7Dh) - Textured Rectangle, 16x16, opaque, raw-texture
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
*/
void GPU::operationGp0TexturedQuad16x16OpaqueRawTexture() {
    Dimensions dimensions = Dimensions(16, 16);
    texturedQuad(dimensions, true, TextureBlendModeRawTexture);
    return;
}

/*
GP0(7Eh) - Textured Rectangle, 16x16, semi-transparent, texture-blending
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
*/
void GPU::operationGp0TexturedQuad16x16SemiTransparentTextureBlending() {
    Dimensions dimensions = Dimensions(16, 16);
    texturedQuad(dimensions, false, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(7Fh) - Textured Rectangle, 16x16, semi-transparent, raw-texture
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
*/
void GPU::operationGp0TexturedQuad16x16SemiTransparentRawTexture() {
    Dimensions dimensions = Dimensions(16, 16);
    texturedQuad(dimensions, false, TextureBlendModeRawTexture);
    return;
}

/*
GP1(02h) - Acknowledge GPU Interrupt (IRQ1)
0-23  Not used (zero)                                        ;GPUSTAT.24
*/
void GPU::operationGp1AcknowledgeGPUInterrupt(uint32_t value) {
    interruptRequestEnable = false;
    return;
}

/*
GP1(01h) - Reset Command Buffer
0-23  Not used (zero)
*/
void GPU::operationGp1ResetCommandBuffer(uint32_t value) {
    gp0InstructionBuffer.clear();
    gp0WordsRemaining = 0;
    gp0Mode = GP0Mode::Command;
    printWarning("TODO: clear the command FIFO");
}

/*
GP1(10h) - Get GPU Info
GP1(11h..1Fh) - Mirrors of GP1(10h), Get GPU Info
On New 208pin GPUs, following values can be selected:
00h-01h = Returns Nothing (old value in GPUREAD remains unchanged)
02h     = Read Texture Window setting  ;GP0(E2h) ;20bit/MSBs=Nothing
03h     = Read Draw area top left      ;GP0(E3h) ;20bit/MSBs=Nothing
04h     = Read Draw area bottom right  ;GP0(E4h) ;20bit/MSBs=Nothing
05h     = Read Draw offset             ;GP0(E5h) ;22bit
06h     = Returns Nothing (old value in GPUREAD remains unchanged)
07h     = Read GPU Type (usually 2)    ;see "GPU Versions" chapter
08h     = Unknown (Returns 00000000h) (lightgun on some GPUs?)
09h-0Fh = Returns Nothing (old value in GPUREAD remains unchanged)
10h-FFFFFFh = Mirrors of 00h..0Fh
*/
void GPU::operationGp1GetGPUInfo(uint32_t value) {
    switch (value & 0xf) {
        case 0x0:
        case 0x1: {
            break;
        }
        case 0x2: {
            gpuRead = ((uint32_t) textureWindowOffsetY) << 15 | ((uint32_t) textureWindowOffsetX) << 10 | ((uint32_t) textureWindowMaskY) << 5 | ((uint32_t) textureWindowMaskX);
            break;
        }
        case 0x3: {
            gpuRead = ((uint32_t) drawingAreaTop) << 10 | ((uint32_t) drawingAreaLeft);
            break;
        }
        case 0x4: {
            gpuRead = ((uint32_t) drawingAreaBottom) << 10 | ((uint32_t) drawingAreaRight);
            break;
        }
        case 0x5: {
            uint32_t x = ((uint32_t)drawingOffsetX) & 0x7ff;
            uint32_t y = ((uint32_t)drawingOffsetY) & 0x7ff;
            gpuRead = y << 11 | x;
            break;
        }
        case 0x6: {
            break;
        }
        case 0x7: {
            gpuRead = 0x2;
            break;
        }
        case 0x8: {
            gpuRead = 0x0;
            break;
        }
        case 0x9:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f: {
            break;
        }
    }
}

/*
GP0(68h) - Monochrome Rectangle (1x1) (Dot) (opaque)
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex            (YyyyXxxxh)
*/
void GPU::operationGp0MonochromeRectangle1x1DotOpaque() {
    Color color = Color(gp0InstructionBuffer[0]);
    Point point = Point(gp0InstructionBuffer[1]);
    Vertex topLeft = Vertex(point, color);
    Vertex topRight = Vertex(point, color);
    topRight.point.x += + 1;
    Vertex bottomLeft = Vertex(point, color);
    bottomLeft.point.y += 1;
    Vertex bottomRight = Vertex(point, color);
    bottomRight.point.x += 1;
    bottomRight.point.y += 1;
    array<Vertex, 4> vertices = {
        topLeft,
        topRight,
        bottomLeft,
        bottomRight,
    };
    renderer.pushQuad(vertices);
    return;
}

/*
GP0(02h) - Fill Rectangle in VRAM
1st  Color+Command     (CcBbGgRrh)  ;24bit RGB value (see note)
2nd  Top Left Corner   (YyyyXxxxh)  ;Xpos counted in halfwords, steps of 10h
3rd  Width+Height      (YsizXsizh)  ;Xsiz counted in halfwords, steps of 10h
*/
void GPU::operationGp0FillRectagleInVRAM() {
    Color color = Color(gp0InstructionBuffer[0]);
    Point point = Point(gp0InstructionBuffer[1]);
    Dimensions dimentions = Dimensions(gp0InstructionBuffer[2]);
    Vertex topLeft = Vertex(point, color);
    uint32_t width = dimentions.width;
    uint32_t height = dimentions.height;
    Vertex topRight = Vertex(gp0InstructionBuffer[1], color);
    topRight.point.x = topRight.point.x + width;
    Vertex bottomLeft = Vertex(gp0InstructionBuffer[1], color);
    bottomLeft.point.y = bottomLeft.point.y + height;
    Vertex bottomRight = Vertex(gp0InstructionBuffer[1], color);
    bottomRight.point.x = bottomRight.point.x + width;
    bottomRight.point.y = bottomRight.point.y + height;
    array<Vertex, 4> vertices = {
        topLeft,
        topRight,
        bottomLeft,
        bottomRight,
    };
    renderer.pushQuad(vertices);
    return;
}

void GPU::texturedQuad(Dimensions dimensions, bool opaque, TextureBlendMode textureBlendMode) {
    Color color = Color(gp0InstructionBuffer[0]);
    Point point1 = Point(gp0InstructionBuffer[1]);
    Point texturePoint1 = Point::forTexturePosition(gp0InstructionBuffer[2] & 0xffff);
    Point point2 = Point(gp0InstructionBuffer[1]);
    point2.x += dimensions.width;
    Point texturePoint2 = Point::forTexturePosition(gp0InstructionBuffer[2] & 0xffff);
    texturePoint2.x += dimensions.width;
    Point point3 = Point(gp0InstructionBuffer[1]);
    point3.y += dimensions.height;
    Point texturePoint3 = Point::forTexturePosition(gp0InstructionBuffer[2] & 0xffff);
    texturePoint3.y += dimensions.height;
    Point point4 = Point(gp0InstructionBuffer[1]);
    point4.x += dimensions.width;
    point4.y += dimensions.height;
    Point texturePoint4 = Point::forTexturePosition(gp0InstructionBuffer[2] & 0xffff);
    texturePoint4.x += dimensions.width;
    texturePoint4.y += dimensions.height;
    uint16_t texturePageData = texturePageBaseY;
    texturePageData <<= 2;
    texturePageData |= texturePageBaseX;
    Point texturePage = Point::forTexturePage(texturePageData);
    GLuint textureDepthShift = 2 - texturePageColors;
    Point clut = Point::forClut(gp0InstructionBuffer[2] >> 16);
    array<Vertex, 4> vertices = {
        Vertex(point1, color, texturePoint1, textureBlendMode, texturePage, textureDepthShift, clut),
        Vertex(point2, color, texturePoint2, textureBlendMode, texturePage, textureDepthShift, clut),
        Vertex(point3, color, texturePoint3, textureBlendMode, texturePage, textureDepthShift, clut),
        Vertex(point4, color, texturePoint4, textureBlendMode, texturePage, textureDepthShift, clut),
    };
    renderer.pushQuad(vertices);
    return;
}
