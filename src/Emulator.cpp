#include "Emulator.hpp"
#include "TestRunner.hpp"
#include <iostream>
#include "Constants.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include "Output.hpp"

using namespace std;

const uint32_t BIOS_A_FUNCTIONS_STEP = 0xB0;
const uint32_t BIOS_STD_OUT_PUT_CHAR = 0x3D;
const uint32_t SCREEN_WIDTH = 1024;
const uint32_t SCREEN_HEIGHT = 768;

Emulator::Emulator() : ttyBuffer() {
    setupSDL();
    uint32_t screenHeight = SCREEN_HEIGHT;
    TestRunner *testRunner = TestRunner::getInstance();
    if (testRunner->shouldResizeWindowToFitFramebuffer()) {
        screenHeight = 512;
    }
    debugWindow = make_unique<Window>(false, "ルビィ - dbginfo", SCREEN_WIDTH, screenHeight);
    mainWindow = make_unique<Window>(true, "ルビィ", SCREEN_WIDTH, screenHeight);
    mainWindow->makeCurrent();
    setupOpenGL();
    cop0 = make_unique<COP0>();
    bios = make_unique<BIOS>();
    ram = make_unique<RAM>();
    gpu = make_unique<GPU>(mainWindow);
    dma = make_unique<DMA>(ram, gpu);
    scratchpad = make_unique<Scratchpad>();
    cdrom = make_unique<CDROM>();
    interruptController = make_unique<InterruptController>(cop0);
    expansion1 = make_unique<Expansion1>();
    timer0 = make_unique<Timer0>();
    timer1 = make_unique<Timer1>();
    timer2 = make_unique<Timer2>();
    interconnect = make_unique<Interconnect>(cop0, bios, ram, gpu, dma, scratchpad, cdrom, interruptController, expansion1, timer0, timer1, timer2);
    cpu = make_unique<CPU>(interconnect, cop0);
}

Emulator::~Emulator() {}

CPU* Emulator::getCPU() {
    return cpu.get();
}

void Emulator::emulateFrame() {
    TestRunner *testRunner = TestRunner::getInstance();
    uint32_t systemClockStep = 21;
    uint32_t videoSystemClockStep = systemClockStep*11/7;
    uint32_t totalSystemClocksThisFrame = 0;
    uint32_t videoSystemClocksScanlineCounter = 0;
    uint32_t totalScanlines = 0;
    while (totalSystemClocksThisFrame < SystemClocksPerSecond) {
        for (uint32_t i = 0; i < systemClockStep / 3; i++) {
            checkTTY();
            if (!cpu->executeNextInstruction()) {
                testRunner->setup();
            }
            totalSystemClocksThisFrame++;
        }
        timer0->step(systemClockStep);
        timer1->step(systemClockStep);
        timer2->step(systemClockStep);
        videoSystemClocksScanlineCounter += videoSystemClockStep;
        if (videoSystemClocksScanlineCounter >= VideoSystemClocksPerScanline) {
            totalScanlines++;
            videoSystemClocksScanlineCounter = 0;
        }
        if (totalScanlines >= ScanlinesPerFrame) {
            interruptController->trigger(VBLANK);
            totalScanlines = 0;
            gpu->render();
            SDL_GL_SwapWindow(mainWindow->getWindowRef());
        }
    }
}

void Emulator::transferToRAM(std::string path, uint32_t origin, uint32_t size, uint32_t destination) {
    interconnect->transferToRAM(path, origin, size, destination);
}

void Emulator::dumpRAM() {
    interconnect->dumpRAM();
}

void Emulator::setupSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printError("Error initializing SDL: %s", SDL_GetError());
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
}

void Emulator::setupOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        printError("Failed to initialize the OpenGL context.");
    }
}

void Emulator::handleSDLEvent(SDL_Event event) {
    mainWindow->handleSDLEvent(event);
    debugWindow->handleSDLEvent(event);
}

bool Emulator::shouldTerminate() {
    return mainWindow->isHidden();
}

void Emulator::checkTTY() {
    if (cpu->getProgramCounter() == BIOS_A_FUNCTIONS_STEP) {
        array<uint32_t, 32> registers = cpu->getRegisters();
        uint32_t function = registers[9];
        if (function == BIOS_STD_OUT_PUT_CHAR) {
            ttyBuffer.append(1, registers[4]);
            if (registers[4] == '\n') {
                cout << ttyBuffer;
                ttyBuffer.clear();
            }
        }
    }
}
