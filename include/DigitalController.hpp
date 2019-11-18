#pragma once
#include <cstdint>
#include "Logger.hpp"
#include <SDL2/SDL.h>

/*
Controller Communication Sequence
Send Reply Comment
01h  Hi-Z  Controller Access (unlike 81h=Memory Card access), dummy response
42h  idlo  Receive ID bit0..7 (variable) and Send Read Command (ASCII "B")
TAP  idhi  Receive ID bit8..15 (usually/always 5Ah)
MOT  swlo  Receive Digital Switches bit0..7
MOT  swhi  Receive Digital Switches bit8..15
--- transfer stops here for digital pad (or analog pad in digital mode) ---
*/
enum CommunicationSequenceStage : uint8_t {
    ControllerAccess = 0,
    ReceiveIDLow = 1,
    ReceiveIDHigh = 2,
    ReceiveDigitalSwitchesLow = 3,
    ReceiveDigitalSwitchesHigh = 4,
};

class DigitalController {
    Logger logger;
    SDL_Joystick *joystick;
    CommunicationSequenceStage currentStage;
    uint16_t identifier;
public:
    DigitalController(LogLevel logLevel);
    ~DigitalController();

    uint8_t getResponse(uint8_t value);
    CommunicationSequenceStage getCurrentStage();
};
