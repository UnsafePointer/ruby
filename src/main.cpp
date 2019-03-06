#include "CPU.hpp"
#include <SDL2/SDL.h>

int main() {
    CPU cpu = CPU();
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
        for (int i = 0; i < 0xFFFF; i++) {
            cpu.executeNextInstruction();
        }
    }
}
