#include <SDL2/SDL.h>
#include <memory>
#include <cstdint>
#include "Emulator.hpp"
#include "Debugger.hpp"
#include "EmulatorRunner.hpp"
#include "Logger.hpp"
#include "Constants.h"
#include "ConfigurationManager.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    EmulatorRunner *emulatorRunner = EmulatorRunner::getInstance();
    emulatorRunner->configure(argc, argv);
    ConfigurationManager *configurationManager = ConfigurationManager::getInstance();
    configurationManager->setupConfigurationFile();
    configurationManager->loadConfiguration();
    std::unique_ptr<Emulator> emulator = std::make_unique<Emulator>();
    emulatorRunner->setEmulator(emulator.get());
    Debugger *debugger = Debugger::getInstance();
    debugger->setCPU(emulator->getCPU());
    Logger *logger = Logger::getInstance();
    logger->configure(configurationManager->shouldTraceLogs(), configurationManager->shouldLogVerbose());
    logger->setupTraceFile();
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
    ConfigurationManager::removeInstance();
    EmulatorRunner::removeInstance();
}
