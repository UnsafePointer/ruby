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
    std::vector<std::string> biosFunctionsLog;
public:
    DebugInfoRenderer(std::unique_ptr<Window> &debugWindow);
    ~DebugInfoRenderer();

    void update();
    void handleSDLEvent(SDL_Event event);
    void pushLog(std::string log);
};
