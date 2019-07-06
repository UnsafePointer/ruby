#include <SDL2/SDL.h>
#include <memory>
#include <cstdint>
#include "Emulator.hpp"
#include "Debugger.hpp"
#include "TestRunner.hpp"
#include "Logger.hpp"

int main(int argc, char* argv[]) {
    TestRunner *testRunner = TestRunner::getInstance();
    testRunner->configure(argc, argv);
    std::unique_ptr<Emulator> emulator = std::make_unique<Emulator>();
    testRunner->setCPU(emulator->getCPU());
    Debugger *debugger = Debugger::getInstance();
    debugger->setCPU(emulator->getCPU());
    Logger *logger = Logger::getInstance();
    logger->configure(testRunner->shouldRunTests());
    logger->setupTraceFile();
    bool quit = false;
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
                }
            }
        }
        if (debugger->isAttached() && debugger->shouldStep()) {
            debugger->doStep();
            continue;
        }
        if (debugger->isAttached() && debugger->isStopped()) {
            continue;
        }
        emulator->emulateFrame();
    }
}
