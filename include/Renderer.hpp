#pragma once
#include <SDL2/SDL.h>
#include "Point.hpp"
#include "Color.hpp"

class Renderer {
    SDL_GLContext glContext;
    SDL_Window *window;
public:
    Renderer();
    ~Renderer();

    void pushTriangle(Point (&points)[3], Color (&colors)[3]);
};
