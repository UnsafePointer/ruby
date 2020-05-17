#include "GPU.hpp"
#include "Vertex.hpp"
#include "Constants.h"
#include "ConfigurationManager.hpp"
#include <iostream>
#include <sstream>

using namespace std;

const uint32_t GP0_COMMAND_TERMINATION_CODE = 0x55555555;

GPU::GPU(LogLevel logLevel, std::unique_ptr<Window> &mainWindow, std::unique_ptr<InterruptController> &interruptController, std::unique_ptr<DebugInfoRenderer> &debugInfoRenderer) : logger(logLevel),
             texturePageBaseX(0),
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
             gp0WordsRead(0),
             gp0InstructionMethod(nullptr),
             gp0Mode(GP0Mode::Command),
             imageBuffer(make_unique<GPUImageBuffer>()),
             interruptController(interruptController),
             videoSystemClocksScanlineCounter(0),
             scanlineCounter(0),
             debugInfoRenderer(debugInfoRenderer),
             frameCounter(0)
{
    renderer = make_unique<Renderer>(mainWindow, this);
    ConfigurationManager *configurationManager = ConfigurationManager::getInstance();
    showDebugInfoWindow = configurationManager->shouldShowDebugInfoWindow();
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
    uint32_t dmaRequest = 0;
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
    logger.logMessage("GPUSTAT [R]: %#x", value);
    return value;
}

void GPU::executeGp0(uint32_t value) {
    if (gp0WordsRemaining == 0) {
        gp0WordsRead = 0;
        uint32_t opCode = (value >> 24) & 0xff;
        logger.logMessage("GP0 [W] with opcode: %#x (%#x)", opCode, value);
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
            case 0x20: {
                gp0WordsRemaining = 4;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeThreePointOpaque();
                };
                break;
            }
            case 0x22: {
                gp0WordsRemaining = 4;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeThreePointSemiTransparent();
                };
                break;
            }
            case 0x28: {
                gp0WordsRemaining = 5;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeFourPointOpaque();
                };
                break;
            }
            case 0x2a: {
                gp0WordsRemaining = 5;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeFourPointSemiTransparent();
                };
                break;
            }
            case 0x24: {
                gp0WordsRemaining = 7;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedThreePointOpaqueTextureBlending();
                };
                break;
            }
            case 0x25: {
                gp0WordsRemaining = 7;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedThreePointOpaqueRawTexture();
                };
                break;
            }
            case 0x26: {
                gp0WordsRemaining = 7;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedThreePointSemiTransparentTextureBlending();
                };
                break;
            }
            case 0x27: {
                gp0WordsRemaining = 7;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedThreePointSemiTransparentRawTexture();
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
            case 0x2d: {
                gp0WordsRemaining = 9;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedFourPointOpaqueRawTexture();
                };
                break;
            }
            case 0x2e: {
                gp0WordsRemaining = 9;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedFourPointSemiTransparentTextureBlending();
                };
                break;
            }
            case 0x2f: {
                gp0WordsRemaining = 9;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedFourPointSemiTransparentRawTexture();
                };
                break;
            }
            case 0x30: {
                gp0WordsRemaining = 6;
                gp0InstructionMethod = [&]() {
                    this->operationGp0ShadedThreePointOpaque();
                };
                break;
            }
            case 0x32: {
                gp0WordsRemaining = 6;
                gp0InstructionMethod = [&]() {
                    this->operationGp0ShadedThreePointSemiTransparent();
                };
                break;
            }
            case 0x38: {
                gp0WordsRemaining = 8;
                gp0InstructionMethod = [&]() {
                    this->operationGp0ShadedFourPointOpaque();
                };
                break;
            }
            case 0x3a: {
                gp0WordsRemaining = 8;
                gp0InstructionMethod = [&]() {
                    this->operationGp0ShadedFourPointSemiTransparent();
                };
                break;
            }
            case 0x34: {
                gp0WordsRemaining = 9;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedShadedThreePointOpaqueTextureBlending();
                };
                break;
            }
            case 0x36: {
                gp0WordsRemaining = 9;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedShadedThreePointSemiTransparentTextureBlending();
                };
                break;
            }
            case 0x3c: {
                gp0WordsRemaining = 12;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedShadedFourPointOpaqueTextureBlending();
                };
                break;
            }
            case 0x3e: {
                gp0WordsRemaining = 12;
                gp0InstructionMethod = [&]() {
                    this->operationGp0TexturedShadedFourPointSemiTransparentTextureBlending();
                };
                break;
            }
            case 0x40: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeLineOpaque();
                };
                break;
            }
            case 0x42: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeLineSemiTransparent();
                };
                break;
            }
            case 0x48: {
                gp0WordsRemaining = -1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromePolylineOpaque();
                };
                break;
            }
            case 0x4a: {
                gp0WordsRemaining = -1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromePolylineSemiTransparent();
                };
                break;
            }
            case 0x4c: {
                // TODO: validate opcode with documentation
                gp0WordsRemaining = -1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromePolylineOpaque();
                };
                break;
            }
            case 0x50: {
                gp0WordsRemaining = 4;
                gp0InstructionMethod = [&]() {
                    this->operationGp0ShadedLineOpaque();
                };
                break;
            }
            case 0x52: {
                gp0WordsRemaining = 4;
                gp0InstructionMethod = [&]() {
                    this->operationGp0ShadedLineSemiTransparent();
                };
                break;
            }
            case 0x58: {
                gp0WordsRemaining = -1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0ShadedPolylineOpaque();
                };
                break;
            }
            case 0x5a: {
                gp0WordsRemaining = -1;
                gp0InstructionMethod = [&]() {
                    this->operationGp0ShadedPolylineSemiTransparent();
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
            case 0x60: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeQuadOpaque();
                };
                break;
            }
            case 0x62: {
                gp0WordsRemaining = 3;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeQuadSemiTransparent();
                };
                break;
            }
            case 0x68: {
                gp0WordsRemaining = 2;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeQuad1x1Opaque();
                };
                break;
            }
            case 0x6a: {
                gp0WordsRemaining = 2;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeQuad1x1SemiTransparent();
                };
                break;
            }
            case 0x70: {
                gp0WordsRemaining = 2;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeQuad8x8Opaque();
                };
                break;
            }
            case 0x72: {
                gp0WordsRemaining = 2;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeQuad8x8SemiTransparent();
                };
                break;
            }
            case 0x78: {
                gp0WordsRemaining = 2;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeQuad16x16Opaque();
                };
                break;
            }
            case 0x7a: {
                gp0WordsRemaining = 2;
                gp0InstructionMethod = [&]() {
                    this->operationGp0MonochromeQuad16x16SemiTransparent();
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
                logger.logError("Unhandled gp0 instruction %#x", opCode);
            }
        }
        gp0InstructionBuffer.clear();
    }
    gp0WordsRemaining -= 1;

    if (gp0Mode == GP0Mode::Command) {
        gp0InstructionBuffer.pushWord(value);
        gp0WordsRead++;
        if (gp0WordsRemaining == 0) {
            gp0InstructionMethod();
        }
        if (value == GP0_COMMAND_TERMINATION_CODE) {
            gp0InstructionMethod();
            gp0WordsRemaining = 0;
        }
    } else if (gp0Mode == GP0Mode::ImageLoad) {
        imageBuffer->pushWord(value);
        if (gp0WordsRemaining == 0) {
            renderer->loadImage(imageBuffer);
            gp0Mode = GP0Mode::Command;
        }
    }
}

void GPU::step(uint32_t cycles) {
    uint32_t videoSystemClockStep = cycles*11/7;
    videoSystemClocksScanlineCounter += videoSystemClockStep;
    if (videoSystemClocksScanlineCounter >= VideoSystemClocksPerScanline) {
        scanlineCounter++;
        videoSystemClocksScanlineCounter = 0;
    }
    if (scanlineCounter >= ScanlinesPerFrame) {
        scanlineCounter = 0;
        render();
        interruptController->trigger(VBLANK);
    }
}

void GPU::render() {
    frameCounter++;
    logger.logMessage("Rendering frame: %ld", frameCounter);
    renderer->prepareFrame();
    renderer->renderFrame();
    renderer->finalizeFrame();
    if (showDebugInfoWindow) {
        debugInfoRenderer->update();
        // This application makes most of the OpenGL work on the main window, so after
        // we are doine with the debug window we forget about it until the next time to update
        renderer->resetMainWindow();
    }
    stringstream title = stringstream();
    title << EmulatorName;
    title << " - " << dec << frameCounter << " frames";
    renderer->updateWindowTitle(title.str());
}

void GPU::updateDrawingArea() {
    Point2D topLeft = getDrawingAreaTopLeft();
    Dimensions size = getDrawingAreaSize();
    logger.logMessage("Updating renderer drawing area with top left: x=%d, y=%d and size: w=%d, h=%d", topLeft.x, topLeft.y, size.width, size.height);
    renderer->setDrawingArea(topLeft, size);
}

Dimensions GPU::getResolution() {
    uint32_t verticalResolution = 240;
    if (this->verticalResolution == VerticalResolution::Y480) {
        verticalResolution = 480;
    }
    uint8_t horizontalResolutionValue1 = (horizontalResolution >> 1) & 3;
    uint8_t horizontalResolutionValue2 = horizontalResolution & 1;
    if (horizontalResolutionValue2 == 1) {
        return { 368, verticalResolution };
    } else {
        switch (horizontalResolutionValue1) {
            case 0: {
                return { 256, verticalResolution };
            }
            case 1: {
                return { 320, verticalResolution };
            }
            case 2: {
                return { 512, verticalResolution };
            }
            case 3: {
                return { 640, verticalResolution };
            }
        }
    }
    return { 0, 0 };
}

Point2D GPU::getDisplayAreaStart() {
    return { (int16_t)displayVRAMStartX, (int16_t)displayVRAMStartY };
}

Dimensions GPU::getDrawingAreaSize() {
    uint32_t width = drawingAreaRight - drawingAreaLeft;
    uint32_t height = drawingAreaBottom - drawingAreaTop;
    return { (uint32_t)width, (uint32_t)height };
}

Point2D GPU::getDrawingAreaTopLeft() {
    return { (int16_t)drawingAreaLeft, (int16_t)drawingAreaTop };
}

void GPU::toggleRenderPolygonOneByOne() {
    renderer->toggleRenderPolygonOneByOne();
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
            logger.logError("Unhandled gp1 instruction %#x", opCode);
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
    logger.logMessage("GP1(00h) - Reset GPU: %#x", value);

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
    logger.logMessage("GPUREAD [R]: %#x", gpuRead);
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
    logger.logMessage("GP1(08h) - Display mode: %#x", value);
    uint8_t horizontalResolutionValue1 = (value & 3);
    uint8_t horizontalResolutionValue2 = (value >> 6) & 1;
    horizontalResolution = horizontalResolutionFromValues(horizontalResolutionValue1, horizontalResolutionValue2);
    verticalResolution = VerticalResolution((value >> 2) & 0x1);
    videoMode = VideoMode((value >> 3) & 0x1);
    displayAreaColorDepth = DisplayAreaColorDepth((value >> 4) & 0x1);
    verticalInterlaceEnable = (value >> 5) & 0x1;
    if ((value & 0x80) != 0) {
        // This is supposed to be bit 14 on GPUSTAT
        logger.logError("Unsupported display mode: distorted");
    }
    renderer->setScreenResolution(getResolution());
}

/*
GP1(04h) - DMA Direction / Data Request
0-1  DMA Direction (0=Off, 1=FIFO, 2=CPUtoGP0, 3=GPUREADtoCPU) ;GPUSTAT.29-30
2-23 Not used (zero)
*/
void GPU::operationGp1DMADirection(uint32_t value) {
    logger.logMessage("GP1(04h) - DMA Direction / Data Request: %#x", value);
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
    updateDrawingArea();
    logger.logMessage("GP0(E3h) - Set Drawing Area top left: %d, %d", drawingAreaTop, drawingAreaLeft);
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
    updateDrawingArea();
    logger.logMessage("GP0(E3h) - Set Drawing Area bottom right: %d, %d", drawingAreaBottom, drawingAreaRight);
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

    drawingOffsetX = ((int16_t)(x << 5)) >> 5;
    drawingOffsetY = ((int16_t)(y << 5)) >> 5;

    renderer->setDrawingOffset(drawingOffsetX, drawingOffsetY);
    logger.logMessage("GP0(E3h) - Set Drawing Offset: %d, %d", drawingOffsetX, drawingOffsetY);
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
    logger.logMessage("GP1(05h) - Start of Display area (in VRAM): %#x", value);
    displayVRAMStartX = (value & 0x3fe);
    displayVRAMStartY = ((value >> 10) & 0x1ff);
    renderer->setDisplayAreaSart(getDisplayAreaStart());
}

/*
GP1(06h) - Horizontal Display range (on Screen)
0-11   X1 (260h+0)       ;12bit       ;\counted in 53.222400MHz units,
12-23  X2 (260h+320*8)   ;12bit       ;/relative to HSYNC
*/
void GPU::operationGp1HorizontalDisplayRange(uint32_t value) {
    logger.logMessage("GP1(06h) - Horizontal Display range (on Screen): %#x", value);
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
    logger.logMessage("GP1(07h) - Vertical Display range (on Screen): %#x", value);
    displayLineStart = (value & 0x3ff);
    displayLineEnd = ((value >> 10) & 0x3ff);
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
    Point2D point = Point2D(gp0InstructionBuffer[1]);
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
    logger.logMessage("GP1(03h) - Display Enable: %#x", value);
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

    logger.logWarning("Unhandled GP0 Copy Rectangle VRAM to CPU with with resolution: %d x %d", width, height);
}

/*
GP0(20h) - Monochrome three-point polygon, opaque
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex1           (YyyyXxxxh)
3rd  Vertex2           (YyyyXxxxh)
4th  Vertex3           (YyyyXxxxh)
*/
void GPU::operationGp0MonochromeThreePointOpaque() {
    monochromePolygon(3, true);
    return;
}

/*
GP0(22h) - Monochrome three-point polygon, semi-transparent
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex1           (YyyyXxxxh)
3rd  Vertex2           (YyyyXxxxh)
4th  Vertex3           (YyyyXxxxh)
*/
void GPU::operationGp0MonochromeThreePointSemiTransparent() {
    monochromePolygon(3, false);
    return;
}

/*
GP0(28h) - Monochrome four-point polygon, opaque
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex1           (YyyyXxxxh)
3rd  Vertex2           (YyyyXxxxh)
4th  Vertex3           (YyyyXxxxh)
(5th) Vertex4           (YyyyXxxxh) (if any)
*/
void GPU::operationGp0MonochromeFourPointOpaque() {
    monochromePolygon(4, true);
    return;
}

/*
GP0(2Ah) - Monochrome four-point polygon, semi-transparent
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex1           (YyyyXxxxh)
3rd  Vertex2           (YyyyXxxxh)
4th  Vertex3           (YyyyXxxxh)
(5th) Vertex4           (YyyyXxxxh) (if any)
*/
void GPU::operationGp0MonochromeFourPointSemiTransparent() {
    monochromePolygon(4, false);
    return;
}

/*
GP0(24h) - Textured three-point polygon, opaque, texture-blending
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex1           (YyyyXxxxh)
3rd  Texcoord1+Palette (ClutYyXxh)
4th  Vertex2           (YyyyXxxxh)
5th  Texcoord2+Texpage (PageYyXxh)
6th  Vertex3           (YyyyXxxxh)
7th  Texcoord3         (0000YyXxh)
*/
void GPU::operationGp0TexturedThreePointOpaqueTextureBlending() {
    texturedPolygon(3, true, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(25h) - Textured three-point polygon, opaque, raw-texture
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex1           (YyyyXxxxh)
3rd  Texcoord1+Palette (ClutYyXxh)
4th  Vertex2           (YyyyXxxxh)
5th  Texcoord2+Texpage (PageYyXxh)
6th  Vertex3           (YyyyXxxxh)
7th  Texcoord3         (0000YyXxh)
*/
void GPU::operationGp0TexturedThreePointOpaqueRawTexture() {
    texturedPolygon(3, true, TextureBlendModeRawTexture);
    return;
}

/*
GP0(26h) - Textured three-point polygon, semi-transparent, texture-blending
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex1           (YyyyXxxxh)
3rd  Texcoord1+Palette (ClutYyXxh)
4th  Vertex2           (YyyyXxxxh)
5th  Texcoord2+Texpage (PageYyXxh)
6th  Vertex3           (YyyyXxxxh)
7th  Texcoord3         (0000YyXxh)
*/
void GPU::operationGp0TexturedThreePointSemiTransparentTextureBlending() {
    texturedPolygon(3, false, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(27h) - Textured three-point polygon, semi-transparent, raw-texture
1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
2nd  Vertex1           (YyyyXxxxh)
3rd  Texcoord1+Palette (ClutYyXxh)
4th  Vertex2           (YyyyXxxxh)
5th  Texcoord2+Texpage (PageYyXxh)
6th  Vertex3           (YyyyXxxxh)
7th  Texcoord3         (0000YyXxh)
*/
void GPU::operationGp0TexturedThreePointSemiTransparentRawTexture() {
    texturedPolygon(3, false, TextureBlendModeRawTexture);
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
    texturedPolygon(4, true, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(2Dh) - Textured four-point polygon, opaque, raw-texture
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
void GPU::operationGp0TexturedFourPointOpaqueRawTexture() {
    texturedPolygon(4, true, TextureBlendModeRawTexture);
    return;
}

/*
GP0(2Eh) - Textured four-point polygon, semi-transparent, texture-blending
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
void GPU::operationGp0TexturedFourPointSemiTransparentTextureBlending() {
    texturedPolygon(4, false, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(2Fh) - Textured four-point polygon, semi-transparent, raw-texture
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
void GPU::operationGp0TexturedFourPointSemiTransparentRawTexture() {
    texturedPolygon(4, false, TextureBlendModeRawTexture);
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
void GPU::operationGp0ShadedThreePointOpaque() {
    shadedPolygon(3, true);
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
void GPU::operationGp0ShadedThreePointSemiTransparent() {
    shadedPolygon(3, false);
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
 (7th) Color4            (00BbGgRrh) (if any)
 (8th) Vertex4           (YyyyXxxxh) (if any)

*/
void GPU::operationGp0ShadedFourPointOpaque() {
    shadedPolygon(4, true);
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
 (7th) Color4            (00BbGgRrh) (if any)
 (8th) Vertex4           (YyyyXxxxh) (if any)
*/
void GPU::operationGp0ShadedFourPointSemiTransparent() {
    shadedPolygon(4, false);
    return;
}

/*
GP0(34h) - Shaded Textured three-point polygon, opaque, texture-blending
  1st  Color1+Command    (CcBbGgRrh)
  2nd  Vertex1           (YyyyXxxxh)
  3rd  Texcoord1+Palette (ClutYyXxh)
  4th  Color2            (00BbGgRrh)
  5th  Vertex2           (YyyyXxxxh)
  6th  Texcoord2+Texpage (PageYyXxh)
  7th  Color3            (00BbGgRrh)
  8th  Vertex3           (YyyyXxxxh)
  9th  Texcoord3         (0000YyXxh)
*/
void GPU::operationGp0TexturedShadedThreePointOpaqueTextureBlending() {
    shadedTexturedPolygon(3, true, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(36h) - Shaded Textured three-point polygon, semi-transparent, tex-blend
  1st  Color1+Command    (CcBbGgRrh)
  2nd  Vertex1           (YyyyXxxxh)
  3rd  Texcoord1+Palette (ClutYyXxh)
  4th  Color2            (00BbGgRrh)
  5th  Vertex2           (YyyyXxxxh)
  6th  Texcoord2+Texpage (PageYyXxh)
  7th  Color3            (00BbGgRrh)
  8th  Vertex3           (YyyyXxxxh)
  9th  Texcoord3         (0000YyXxh)
*/
void GPU::operationGp0TexturedShadedThreePointSemiTransparentTextureBlending() {
    shadedTexturedPolygon(3, false, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(3Ch) - Shaded Textured four-point polygon, opaque, texture-blending
  1st  Color1+Command    (CcBbGgRrh)
  2nd  Vertex1           (YyyyXxxxh)
  3rd  Texcoord1+Palette (ClutYyXxh)
  4th  Color2            (00BbGgRrh)
  5th  Vertex2           (YyyyXxxxh)
  6th  Texcoord2+Texpage (PageYyXxh)
  7th  Color3            (00BbGgRrh)
  8th  Vertex3           (YyyyXxxxh)
  9th  Texcoord3         (0000YyXxh)
 (10th) Color4           (00BbGgRrh) (if any)
 (11th) Vertex4          (YyyyXxxxh) (if any)
 (12th) Texcoord4        (0000YyXxh) (if any)
*/
void GPU::operationGp0TexturedShadedFourPointOpaqueTextureBlending() {
    shadedTexturedPolygon(4, true, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(3Eh) - Shaded Textured four-point polygon, semi-transparent, tex-blend
  1st  Color1+Command    (CcBbGgRrh)
  2nd  Vertex1           (YyyyXxxxh)
  3rd  Texcoord1+Palette (ClutYyXxh)
  4th  Color2            (00BbGgRrh)
  5th  Vertex2           (YyyyXxxxh)
  6th  Texcoord2+Texpage (PageYyXxh)
  7th  Color3            (00BbGgRrh)
  8th  Vertex3           (YyyyXxxxh)
  9th  Texcoord3         (0000YyXxh)
 (10th) Color4           (00BbGgRrh) (if any)
 (11th) Vertex4          (YyyyXxxxh) (if any)
 (12th) Texcoord4        (0000YyXxh) (if any)
*/
void GPU::operationGp0TexturedShadedFourPointSemiTransparentTextureBlending() {
    shadedTexturedPolygon(4, true, TextureBlendModeTextureBlend);
    return;
}

/*
GP0(40h) - Monochrome line, opaque
  1st   Color+Command     (CcBbGgRrh)
  2nd   Vertex1           (YyyyXxxxh)
  3rd   Vertex2           (YyyyXxxxh)
*/
void GPU::operationGp0MonochromeLineOpaque() {
    monochromeLine(2, true);
    return;
}

/*
GP0(42h) - Monochrome line, semi-transparent
  1st   Color+Command     (CcBbGgRrh)
  2nd   Vertex1           (YyyyXxxxh)
  3rd   Vertex2           (YyyyXxxxh)
*/
void GPU::operationGp0MonochromeLineSemiTransparent() {
    monochromeLine(2, false);
    return;
}

/*
GP0(48h) - Monochrome Poly-line, opaque
  1st   Color+Command     (CcBbGgRrh)
  2nd   Vertex1           (YyyyXxxxh)
  3rd   Vertex2           (YyyyXxxxh)
 (...)  VertexN           (YyyyXxxxh) (poly-line only)
 (Last) Termination Code  (55555555h) (poly-line only)
*/
void GPU::operationGp0MonochromePolylineOpaque() {
    monochromeLine(gp0WordsRead - 2, true);
    return;
}

/*
GP0(4Ah) - Monochrome Poly-line, semi-transparent
  1st   Color+Command     (CcBbGgRrh)
  2nd   Vertex1           (YyyyXxxxh)
  3rd   Vertex2           (YyyyXxxxh)
 (...)  VertexN           (YyyyXxxxh) (poly-line only)
 (Last) Termination Code  (55555555h) (poly-line only)
*/
void GPU::operationGp0MonochromePolylineSemiTransparent() {
    monochromeLine(gp0WordsRead - 2, true);
    return;
}

/*
GP0(50h) - Shaded line, opaque
  1st   Color1+Command    (CcBbGgRrh)
  2nd   Vertex1           (YyyyXxxxh)
  3rd   Color2            (00BbGgRrh)
  4th   Vertex2           (YyyyXxxxh)
*/
void GPU::operationGp0ShadedLineOpaque() {
    shadedLine(2, true);
    return;
}

/*
GP0(52h) - Shaded line, semi-transparent
  1st   Color1+Command    (CcBbGgRrh)
  2nd   Vertex1           (YyyyXxxxh)
  3rd   Color2            (00BbGgRrh)
  4th   Vertex2           (YyyyXxxxh)
*/
void GPU::operationGp0ShadedLineSemiTransparent() {
    shadedLine(2, false);
    return;
}

/*
GP0(58h) - Shaded Poly-line, opaque
  1st   Color1+Command    (CcBbGgRrh)
  2nd   Vertex1           (YyyyXxxxh)
  3rd   Color2            (00BbGgRrh)
  4th   Vertex2           (YyyyXxxxh)
 (...)  ColorN            (00BbGgRrh) (poly-line only)
 (...)  VertexN           (YyyyXxxxh) (poly-line only)
 (Last) Termination Code  (55555555h) (poly-line only)
*/
void GPU::operationGp0ShadedPolylineOpaque()  {
    shadedLine((gp0WordsRead - 1) / 2, true);
    return;
}

/*
GP0(5Ah) - Shaded Poly-line, semi-transparent
  1st   Color1+Command    (CcBbGgRrh)
  2nd   Vertex1           (YyyyXxxxh)
  3rd   Color2            (00BbGgRrh)
  4th   Vertex2           (YyyyXxxxh)
 (...)  ColorN            (00BbGgRrh) (poly-line only)
 (...)  VertexN           (YyyyXxxxh) (poly-line only)
 (Last) Termination Code  (55555555h) (poly-line only)
*/
void GPU::operationGp0ShadedPolylineSemiTransparent() {
    shadedLine((gp0WordsRead - 1) / 2, false);
    return;
}

/*
GP0(60h) - Monochrome Rectangle (variable size) (opaque)
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex            (YyyyXxxxh)
(3rd) Width+Height      (YsizXsizh) (variable size only) (max 1023x511)
*/
void GPU::operationGp0MonochromeQuadOpaque() {
    Dimensions dimensions = Dimensions(gp0InstructionBuffer[2]);
    quad(dimensions, true);
    return;
}

/*
GP0(62h) - Monochrome Rectangle (variable size) (semi-transparent)
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex            (YyyyXxxxh)
(3rd) Width+Height      (YsizXsizh) (variable size only) (max 1023x511)
*/
void GPU::operationGp0MonochromeQuadSemiTransparent() {
    Dimensions dimensions = Dimensions(gp0InstructionBuffer[2]);
    quad(dimensions, false);
    return;
}

/*
GP0(68h) - Monochrome Rectangle (1x1) (Dot) (opaque)
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex            (YyyyXxxxh)
*/
void GPU::operationGp0MonochromeQuad1x1Opaque() {
    Dimensions dimensions = Dimensions(1, 1);
    quad(dimensions, true);
    return;
}

/*
GP0(6Ah) - Monochrome Rectangle (1x1) (Dot) (semi-transparent)
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex            (YyyyXxxxh)
*/
void GPU::operationGp0MonochromeQuad1x1SemiTransparent() {
    Dimensions dimensions = Dimensions(1, 1);
    quad(dimensions, false);
    return;
}

/*
GP0(70h) - Monochrome Rectangle (8x8) (opaque)
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex            (YyyyXxxxh)
*/
void GPU::operationGp0MonochromeQuad8x8Opaque() {
    Dimensions dimensions = Dimensions(8, 8);
    quad(dimensions, true);
    return;
}

/*
GP0(72h) - Monochrome Rectangle (8x8) (semi-transparent)
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex            (YyyyXxxxh)
*/
void GPU::operationGp0MonochromeQuad8x8SemiTransparent() {
    Dimensions dimensions = Dimensions(8, 8);
    quad(dimensions, false);
    return;
}

/*
GP0(78h) - Monochrome Rectangle (16x16) (opaque)
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex            (YyyyXxxxh)
*/
void GPU::operationGp0MonochromeQuad16x16Opaque() {
    Dimensions dimensions = Dimensions(16, 16);
    quad(dimensions, true);
    return;
}

/*
GP0(7Ah) - Monochrome Rectangle (16x16) (semi-transparent)
1st  Color+Command     (CcBbGgRrh)
2nd  Vertex            (YyyyXxxxh)
*/
void GPU::operationGp0MonochromeQuad16x16SemiTransparent() {
    Dimensions dimensions = Dimensions(16, 16);
    quad(dimensions, false);
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
    logger.logMessage("GP1(02h) - Acknowledge GPU Interrupt (IRQ1): %#x", value);
    interruptRequestEnable = false;
    return;
}

/*
GP1(01h) - Reset Command Buffer
0-23  Not used (zero)
*/
void GPU::operationGp1ResetCommandBuffer(uint32_t value) {
    logger.logMessage("GP1(01h) - Reset Command Buffer: %#x", value);

    gp0InstructionBuffer.clear();
    gp0WordsRemaining = 0;
    gp0Mode = GP0Mode::Command;
    logger.logWarning("TODO: clear the command FIFO");
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
    logger.logMessage("GP1(10h) - Get GPU Info: %#x", value);
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
GP0(02h) - Fill Rectangle in VRAM
1st  Color+Command     (CcBbGgRrh)  ;24bit RGB value (see note)
2nd  Top Left Corner   (YyyyXxxxh)  ;Xpos counted in halfwords, steps of 10h
3rd  Width+Height      (YsizXsizh)  ;Xsiz counted in halfwords, steps of 10h
*/
void GPU::operationGp0FillRectagleInVRAM() {
    Color color = Color(gp0InstructionBuffer[0]);
    Point2D point = Point2D(gp0InstructionBuffer[1]);
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
    vector<Vertex> vertices = {
        topLeft,
        topRight,
        bottomLeft,
        bottomRight,
    };
    renderer->setDrawingOffset(0, 0);
    renderer->pushPolygon(vertices);
    renderer->setDrawingOffset(drawingOffsetX, drawingOffsetY);
    return;
}

void GPU::texturedQuad(Dimensions dimensions, bool opaque, TextureBlendMode textureBlendMode) {
    // TODO: unused
    (void)opaque;
    (void)textureBlendMode;
    Color color = Color(gp0InstructionBuffer[0]);
    Point2D point1 = Point2D(gp0InstructionBuffer[1]);
    Point2D texturePoint1 = Point2D::forTexturePosition(gp0InstructionBuffer[2] & 0xffff);
    Point2D point2 = Point2D(gp0InstructionBuffer[1]);
    point2.x += dimensions.width;
    Point2D texturePoint2 = Point2D::forTexturePosition(gp0InstructionBuffer[2] & 0xffff);
    texturePoint2.x += dimensions.width;
    Point2D point3 = Point2D(gp0InstructionBuffer[1]);
    point3.y += dimensions.height;
    Point2D texturePoint3 = Point2D::forTexturePosition(gp0InstructionBuffer[2] & 0xffff);
    texturePoint3.y += dimensions.height;
    Point2D point4 = Point2D(gp0InstructionBuffer[1]);
    point4.x += dimensions.width;
    point4.y += dimensions.height;
    Point2D texturePoint4 = Point2D::forTexturePosition(gp0InstructionBuffer[2] & 0xffff);
    texturePoint4.x += dimensions.width;
    texturePoint4.y += dimensions.height;
    uint16_t texturePageData = texturePageBaseY;
    texturePageData <<= 4;
    texturePageData |= texturePageBaseX;
    Point2D texturePage = Point2D::forTexturePage(texturePageData);
    GLuint textureDepthShift = 2 - texturePageColors;
    Point2D clut = Point2D::forClut(gp0InstructionBuffer[2] >> 16);
    vector<Vertex> vertices = {
        Vertex(point1, color, texturePoint1, textureBlendMode, texturePage, textureDepthShift, clut),
        Vertex(point2, color, texturePoint2, textureBlendMode, texturePage, textureDepthShift, clut),
        Vertex(point3, color, texturePoint3, textureBlendMode, texturePage, textureDepthShift, clut),
        Vertex(point4, color, texturePoint4, textureBlendMode, texturePage, textureDepthShift, clut),
    };
    renderer->pushPolygon(vertices);
    return;
}

void GPU::quad(Dimensions dimensions, bool opaque) {
    // TODO: unused
    (void)opaque;
    Color color = Color(gp0InstructionBuffer[0]);
    Point2D point = Point2D(gp0InstructionBuffer[1]);
    Vertex topLeft = Vertex(point, color);
    Vertex topRight = Vertex(point, color);
    topRight.point.x += + dimensions.width;
    Vertex bottomLeft = Vertex(point, color);
    bottomLeft.point.y += dimensions.height;
    Vertex bottomRight = Vertex(point, color);
    bottomRight.point.x += dimensions.width;
    bottomRight.point.y += dimensions.height;
    vector<Vertex> vertices = {
        topLeft,
        topRight,
        bottomLeft,
        bottomRight,
    };
    renderer->pushPolygon(vertices);
    return;
}

void GPU::monochromePolygon(unsigned int numberOfPoints, bool opaque) {
    // TODO: unused
    (void)opaque;
    Color color = Color(gp0InstructionBuffer[0]);
    vector<Vertex> vertices = vector<Vertex>();
    for (unsigned int i = 1; i <= numberOfPoints; i++) {
        Point2D point = Point2D(gp0InstructionBuffer[i]);
        vertices.push_back(Vertex(point, color));
    }
    renderer->pushPolygon(vertices);
}

void GPU::shadedPolygon(unsigned int numberOfPoints, bool opaque) {
    // TODO: unused
    (void)opaque;
    vector<Vertex> vertices = vector<Vertex>();
    for (unsigned int i = 0; i < numberOfPoints; i++) {
        Color color = Color(gp0InstructionBuffer[i*2]);
        Point2D point = Point2D(gp0InstructionBuffer[i*2+1]);
        vertices.push_back(Vertex(point, color));
    }
    renderer->pushPolygon(vertices);
}

void GPU::texturedPolygon(unsigned int numberOfPoints, bool opaque, TextureBlendMode textureBlendMode) {
    // TODO: unused
    (void)opaque;
    (void)textureBlendMode;
    Color color = Color(gp0InstructionBuffer[0]);
    Point2D clut = Point2D::forClut(gp0InstructionBuffer[2] >> 16);
    Point2D texturePage = Point2D::forTexturePage(gp0InstructionBuffer[4] >> 16);
    TexturePageColors texturePageColors = texturePageColorsWithValue(((gp0InstructionBuffer[4] >> 16) >> 7) & 0x3);
    GLuint textureDepthShift = 2 - texturePageColors;

    vector<Vertex> vertices = vector<Vertex>();
    for (unsigned int i = 0; i < numberOfPoints; i++) {
        Point2D point = Point2D(gp0InstructionBuffer[i*2+1]);
        Point2D texturePoint = Point2D::forTexturePosition(gp0InstructionBuffer[i*2+2] & 0xffff);
        Vertex vertex = Vertex(point, color, texturePoint, textureBlendMode, texturePage, textureDepthShift, clut);
        vertices.push_back(vertex);
    }
    renderer->pushPolygon(vertices);
}

void GPU::shadedTexturedPolygon(unsigned int numberOfPoints, bool opaque, TextureBlendMode textureBlendMode) {
    // TODO: unused
    (void)opaque;
    (void)textureBlendMode;
    Point2D clut = Point2D::forClut(gp0InstructionBuffer[2] >> 16);
    Point2D texturePage = Point2D::forTexturePage(gp0InstructionBuffer[5] >> 16);
    TexturePageColors texturePageColors = texturePageColorsWithValue(((gp0InstructionBuffer[5] >> 16) >> 7) & 0x3);
    GLuint textureDepthShift = 2 - texturePageColors;
    vector<Vertex> vertices = vector<Vertex>();
    for (unsigned int i = 0; i < numberOfPoints; i++) {
        Color color = Color(gp0InstructionBuffer[i*3]);
        Point2D point = Point2D(gp0InstructionBuffer[i*3+1]);
        Point2D texturePoint = Point2D::forTexturePosition(gp0InstructionBuffer[i*3+2] & 0xffff);
        Vertex vertex = Vertex(point, color, texturePoint, textureBlendMode, texturePage, textureDepthShift, clut);
        vertices.push_back(vertex);
    }
    renderer->pushPolygon(vertices);
}

void GPU::monochromeLine(unsigned int numberOfPoints, bool opaque) {
    // TODO: unused
    (void)opaque;
    Color color = Color(gp0InstructionBuffer[0]);
    vector<Vertex> vertices = vector<Vertex>();
    for (unsigned int i = 1; i <= numberOfPoints; i++) {
        Point2D point = Point2D(gp0InstructionBuffer[i]);
        vertices.push_back(Vertex(point, color));
    }
    if (numberOfPoints == 2) {
        renderer->pushLine(vertices);
        return;
    }
    vector<Vertex> lines = vector<Vertex>();
    for (unsigned int i = 0; i < vertices.size() - 1; i++) {
        lines.push_back(vertices[i]);
        lines.push_back(vertices[i+1]);
        renderer->pushLine(lines);
        lines.clear();
    }
}

void GPU::shadedLine(unsigned int numberOfPoints, bool opaque) {
    // TODO: unused
    (void)opaque;
    vector<Vertex> vertices = vector<Vertex>();
    for (unsigned int i = 0; i < numberOfPoints; i++) {
        Color color = Color(gp0InstructionBuffer[i*2]);
        Point2D point = Point2D(gp0InstructionBuffer[i*2+1]);
        vertices.push_back(Vertex(point, color));
    }
    if (numberOfPoints == 2) {
        renderer->pushLine(vertices);
        return;
    }
    vector<Vertex> lines = vector<Vertex>();
    for (unsigned int i = 0; i < vertices.size() - 1; i++) {
        lines.push_back(vertices[i]);
        lines.push_back(vertices[i+1]);
        renderer->pushLine(lines);
        lines.clear();
    }
}

uint8_t GPU::horizontalResolutionFromValues(uint8_t value1, uint8_t value2) const {
    return ((value2 & 1) | ((value1 & 3) << 1));
}

TexturePageColors GPU::texturePageColorsWithValue(uint32_t value) const {
    if (value > TexturePageColors::T15Bit) {
        logger.logError("Attempting to create Texture Page Colors out-of-bounds value: %#x", value);
    }
    return TexturePageColors(value);
}
