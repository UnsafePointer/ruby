#pragma once
#include <memory>
#include "Window.hpp"
#include <imgui/imgui.h>
#include <SDL2/SDL.h>
#include <vector>
#include <string>

class DebugInfoRenderer {
    std::unique_ptr<Window> &debugWindow;
    ImGuiIO *io;
    ImVec4 backgroundColor;
public:
    DebugInfoRenderer(std::unique_ptr<Window> &debugWindow);
    ~DebugInfoRenderer();

    void update(std::vector<std::string> biosFunctionsLog);
    void handleSDLEvent(SDL_Event event);
};
