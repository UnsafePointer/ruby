#include "Window.hpp"

using namespace std;

Window::Window(string title, uint32_t width, uint32_t height) : title(title), width(width), height(height) {
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    glContext = SDL_GL_CreateContext(window);
}

Window::~Window() {
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
}

SDL_Window* Window::getWindowRef() {
    return window;
}

SDL_GLContext Window::getGLContext() {
    return glContext;
}

void Window::makeCurrent() {
    SDL_GL_MakeCurrent(window, glContext);
}

Dimensions Window::getDimensions() {
    return { width, height };
}
