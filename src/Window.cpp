#include "Window.hpp"
#include "ConfigurationManager.hpp"

using namespace std;

Window::Window(bool mainWindow, string title, uint32_t width, uint32_t height, bool hidden) : logger(LogLevel::NoLog), mainWindow(mainWindow), title(title), width(width), height(height), hidden(hidden) {
    Uint32 flags = SDL_WINDOW_OPENGL;
    if (hidden) {
        flags |= SDL_WINDOW_HIDDEN;
    }
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    windowID = SDL_GetWindowID(window);
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

void Window::handleSDLEvent(SDL_Event event) {
    if(event.type != SDL_WINDOWEVENT || event.window.windowID != windowID) {
        return;
    }
    switch (event.window.event) {
        case SDL_WINDOWEVENT_CLOSE: {
            hidden = !hidden;
            if (hidden && !mainWindow) {
                SDL_HideWindow(window);
            }
            if (mainWindow) {
                logger.flush();
            }
            break;
        }
    }
}

bool Window::isHidden() {
    return hidden;
}

void Window::toggleHidden() {
    hidden = !hidden;
    if (!hidden) {
        SDL_ShowWindow(window);
    } else if (!mainWindow) {
        SDL_HideWindow(window);
    }
}
