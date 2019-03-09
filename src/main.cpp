#include <SDL2/SDL.h>
#include <memory>
#include "CPU.hpp"
#include "Debugger.hpp"

int main() {
    std::unique_ptr<Debugger> debugger = std::make_unique<Debugger>();
    std::unique_ptr<CPU> cpu = std::make_unique<CPU>(debugger);
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_BACKSPACE: {
                        debugger->debug(cpu.get());
                        break;
                    }
                }
            }

        }
        for (int i = 0; i < 0xFFFF; i++) {
            cpu->executeNextInstruction();
        }
    }
}
