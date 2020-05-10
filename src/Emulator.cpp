#include "Emulator.hpp"
#include "EmulatorRunner.hpp"
#include "ConfigurationManager.hpp"
#include "Constants.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>

using namespace std;

const uint32_t SCREEN_WIDTH = 1024;
const uint32_t SCREEN_HEIGHT = 768;

Emulator::Emulator() : logger(LogLevel::NoLog), ttyBuffer() {
    setupSDL();
    uint32_t screenHeight = SCREEN_HEIGHT;
    ConfigurationManager *configurationManager = ConfigurationManager::getInstance();
    if (configurationManager->shouldResizeWindowToFitFramebuffer()) {
        screenHeight = 512;
    }
    showDebugInfoWindow = configurationManager->shouldShowDebugInfoWindow();
    debugWindow = make_unique<Window>(false, EmulatorName + " - dbginfo", SCREEN_WIDTH, SCREEN_HEIGHT, !showDebugInfoWindow);
    mainWindow = make_unique<Window>(true, EmulatorName, SCREEN_WIDTH, screenHeight, false);
    mainWindow->makeCurrent();
    setupOpenGL();
    debugInfoRenderer = make_unique<DebugInfoRenderer>(debugWindow);
    cop0 = make_unique<COP0>();
    bios = make_unique<BIOS>(configurationManager->biosLogLevel());
    ram = make_unique<RAM>();
    scratchpad = make_unique<Scratchpad>();
    interruptController = make_unique<InterruptController>(configurationManager->interruptLogLevel(), cop0);
    gpu = make_unique<GPU>(configurationManager->gpuLogLevel(), mainWindow, interruptController, debugInfoRenderer);
    LogLevel cdromLogLevel = configurationManager->cdromLogLevel();
    cdrom = make_unique<CDROM>(cdromLogLevel, interruptController);
    dma = make_unique<DMA>(configurationManager->dmaLogLevel(), ram, gpu, cdrom, interruptController);
    expansion1 = make_unique<Expansion1>();
    timer0 = make_unique<Timer0>();
    timer1 = make_unique<Timer1>();
    timer2 = make_unique<Timer2>();
    controller = make_unique<Controller>(configurationManager->controllerLogLevel(), interruptController);
    spu = make_unique<SPU>(configurationManager->spuLogLevel());
    interconnect = make_unique<Interconnect>(configurationManager->interconnectLogLevel(), cop0, bios, ram, gpu, dma, scratchpad, cdrom, interruptController, expansion1, timer0, timer1, timer2, controller, spu);
    gte = make_unique<GTE>(configurationManager->gteLogLevel());
    cpu = make_unique<CPU>(configurationManager->cpuLogLevel(), interconnect, cop0, logBiosFunctionCalls, gte);
}

Emulator::~Emulator() {}

CPU* Emulator::getCPU() {
    return cpu.get();
}

void Emulator::emulateFrame() {
    // Emulate cpu for given time slice (21 * magicNumber cycles),
    // then check what events occured during that time slice,
    // finally simulate rest of hardware to accommodate for that
    uint32_t emulationMagicNumber = 4;
    uint32_t systemClockStep = 21 * emulationMagicNumber;
    uint32_t totalSystemClocksThisFrame = 0;
    while (totalSystemClocksThisFrame < SystemClocksPerFrame) {
        for (uint32_t i = 0; i < systemClockStep / 3; i++) {
            checkBIOSFunctions();
            if (!cpu->executeNextInstruction()) {
                EmulatorRunner *emulatorRunner = EmulatorRunner::getInstance();
                emulatorRunner->setup();
            }
            totalSystemClocksThisFrame++;
        }
        dma->step();
        cdrom->step(systemClockStep);
        controller->step(systemClockStep);
        timer0->step(systemClockStep);
        timer1->step(systemClockStep);
        timer2->step(systemClockStep);
        gpu->step(systemClockStep);
    }
}

void Emulator::transferToRAM(filesystem::path filePath, uint32_t origin, uint32_t size, uint32_t destination) {
    interconnect->transferToRAM(filePath, origin, size, destination);
}

void Emulator::dumpRAM() {
    interconnect->dumpRAM();
}

void Emulator::setupSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        logger.logError("Error initializing SDL: %s", SDL_GetError());
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
}

void Emulator::setupOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        logger.logError("Failed to initialize the OpenGL context.");
    }
}

void Emulator::handleSDLEvent(SDL_Event event) {
    mainWindow->handleSDLEvent(event);
    controller->updateInput(event);
    if (showDebugInfoWindow) {
        debugWindow->handleSDLEvent(event);
        debugInfoRenderer->handleSDLEvent(event);
    }
}

bool Emulator::shouldTerminate() {
    return mainWindow->isHidden();
}

void Emulator::toggleDebugInfoWindow() {
    if (!showDebugInfoWindow) {
        return;
    }
    debugWindow->toggleHidden();
}

void Emulator::toggleRenderPolygonOneByOne() {
    gpu->toggleRenderPolygonOneByOne();
}

void Emulator::loadCDROMImageFile(std::filesystem::path filePath) {
    cdrom->loadCDROMImageFile(filePath);
}

void Emulator::checkTTY(char c) {
    if (c == '\n') {
        logger.logDebug("%s", ttyBuffer.c_str());
        ttyBuffer.clear();
        return;
    }
    ttyBuffer.append(1, c);
}

void Emulator::checkBIOSFunctions() {
    array<uint32_t, 32> registers = cpu->getRegisters();
    uint32_t function = registers[9];
    array<uint32_t, 4> subroutineArguments = cpu->getSubroutineArguments();
    optional<string> result = bios->checkFunctions(cpu->getProgramCounter(), function, subroutineArguments);
    if (!result) {
        return;
    }
    string functionCallLog = (*result);
    if (functionCallLog.find("std_out_putchar(char)") == 0) {
        checkTTY(registers[4]);
    }
    if (showDebugInfoWindow) {
        debugInfoRenderer->pushLog(functionCallLog);
    }
}
