#pragma once
#include "Logger.hpp"
#include <SDL2/SDL.h>

class DigitalController {
    Logger logger;
    SDL_Joystick *joystick;
public:
    DigitalController(LogLevel logLevel);
    ~DigitalController();
};
