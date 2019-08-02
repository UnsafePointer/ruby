#include <SDL2/SDL.h>
#include <memory>
#include <cstdint>
#include "Emulator.hpp"
#include "Debugger.hpp"
#include "TestRunner.hpp"
#include "Logger.hpp"
#include <chrono>
#include <thread>
#include "Constants.h"
#include "ConfigurationManager.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    TestRunner *testRunner = TestRunner::getInstance();
    testRunner->configure(argc, argv);
    ConfigurationManager *configurationManager = ConfigurationManager::getInstance();
    configurationManager->setupConfigurationFile();
    std::unique_ptr<Emulator> emulator = std::make_unique<Emulator>();
    testRunner->setEmulator(emulator.get());
    Debugger *debugger = Debugger::getInstance();
    debugger->setCPU(emulator->getCPU());
    Logger *logger = Logger::getInstance();
    logger->configure(testRunner->shouldRunTests(), testRunner->shouldLogVerbose());
    logger->setupTraceFile();
    if (testRunner->shouldSleepAtStartup()) {
        this_thread::sleep_for(chrono::milliseconds(10 * 1000));
    }
    bool quit = false;
    uint32_t initTicks = SDL_GetTicks();
    float interval = 1000;
    interval /= FrameRateTarget;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_BACKSPACE: {
                        debugger->debug();
                        break;
                    }
                    case SDLK_i: {
                        emulator->toggleDebugInfoWindow();
                        break;
                    }
                }
            }
            emulator->handleSDLEvent(event);
        }
        if (debugger->isAttached() && debugger->shouldStep()) {
            debugger->doStep();
            continue;
        }
        if (debugger->isAttached() && debugger->isStopped()) {
            continue;
        }
        if (quit) {
            continue;
        }
        if (emulator->shouldTerminate()) {
            quit = true;
            continue;
        }
        uint32_t currentTicks = SDL_GetTicks();
        if (initTicks + interval < currentTicks) {
            emulator->emulateFrame();
            initTicks = SDL_GetTicks();
        }
    }
}
