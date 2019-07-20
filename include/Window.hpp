#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <cstdint>
#include <Vertex.hpp>

class Window {
    std::string title;
    uint32_t width;
    uint32_t height;
    SDL_GLContext glContext;
    SDL_Window *window;
public:
    Window(std::string title, uint32_t width, uint32_t height);
    ~Window();

    SDL_Window* getWindowRef();
    SDL_GLContext getGLContext();
    void makeCurrent();
    Dimensions getDimensions();
};
