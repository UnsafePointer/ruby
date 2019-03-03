#pragma once
#include <SDL2/SDL.h>

class Renderer {
    SDL_GLContext glContext;
    SDL_Window *window;
public:
    Renderer();
    ~Renderer();
};
