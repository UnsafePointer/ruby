#pragma once
#include <memory>
#include <filesystem>
#include "CPU.hpp"
#include "COP0.hpp"
#include "Interconnect.hpp"
#include "BIOS.hpp"
#include "RAM.hpp"
#include "GPU.hpp"
#include "DMA.hpp"
#include "Scratchpad.hpp"
#include "CDROM.hpp"
#include "InterruptController.hpp"
#include "Expansion1.hpp"
#include "Timer.hpp"
#include "Window.hpp"
#include <SDL2/SDL.h>
#include "DebugInfoRenderer.hpp"
#include <vector>
#include "Controller.hpp"
#include <string>
#include "Logger.hpp"
#include "SPU.hpp"

class Emulator {
    Logger logger;
    std::unique_ptr<Window> mainWindow;
    std::unique_ptr<Window> debugWindow;

    std::unique_ptr<DebugInfoRenderer> debugInfoRenderer;

    std::unique_ptr<CPU> cpu;
    std::unique_ptr<COP0> cop0;
    std::unique_ptr<Interconnect> interconnect;
    std::unique_ptr<BIOS> bios;
    std::unique_ptr<RAM> ram;
    std::unique_ptr<GPU> gpu;
    std::unique_ptr<DMA> dma;
    std::unique_ptr<Scratchpad> scratchpad;
    std::unique_ptr<CDROM> cdrom;
    std::unique_ptr<InterruptController> interruptController;
    std::unique_ptr<Expansion1> expansion1;
    std::unique_ptr<Timer0> timer0;
    std::unique_ptr<Timer1> timer1;
    std::unique_ptr<Timer2> timer2;
    std::unique_ptr<Controller> controller;
    std::unique_ptr<SPU> spu;

    std::string ttyBuffer;
    std::vector<std::string> biosFunctionsLog;

    bool showDebugInfoWindow;
    bool logBiosFunctionCalls;

    void checkBIOSFunctions();
    void checkTTY(char c);
    void setupSDL();
    void setupOpenGL();
public:
    Emulator();
    ~Emulator();

    CPU* getCPU();
    void emulateFrame();
    void transferToRAM(std::filesystem::path filePath, uint32_t origin, uint32_t size, uint32_t destination);
    void dumpRAM();
    void handleSDLEvent(SDL_Event event);
    bool shouldTerminate();
    void toggleDebugInfoWindow();
    void loadCDROMImageFile(std::filesystem::path filePath);
};
