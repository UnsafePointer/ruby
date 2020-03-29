#pragma once
#include <cstdint>
#include "Logger.hpp"
#include <SDL2/SDL.h>

/*
Standard Controllers
__Halfword 0 (Controller Info)_______________________________________________
0-15  Controller Info  (5A41h=digital, 5A73h=analog/pad, 5A53h=analog/stick)
__Halfword 1 (Digital Switches)______________________________________________
0   Select Button    (0=Pressed, 1=Released)
1   L3/Joy-button    (0=Pressed, 1=Released/None/Disabled) ;analog mode only
2   R3/Joy-button    (0=Pressed, 1=Released/None/Disabled) ;analog mode only
3   Start Button     (0=Pressed, 1=Released)
4   Joypad Up        (0=Pressed, 1=Released)
5   Joypad Right     (0=Pressed, 1=Released)
6   Joypad Down      (0=Pressed, 1=Released)
7   Joypad Left      (0=Pressed, 1=Released)
8   L2 Button        (0=Pressed, 1=Released) (Lower-left shoulder)
9   R2 Button        (0=Pressed, 1=Released) (Lower-right shoulder)
10  L1 Button        (0=Pressed, 1=Released) (Upper-left shoulder)
11  R1 Button        (0=Pressed, 1=Released) (Upper-right shoulder)
12  /\ Button        (0=Pressed, 1=Released) (Triangle, upper button)
13  () Button        (0=Pressed, 1=Released) (Circle, right button)
14  >< Button        (0=Pressed, 1=Released) (Cross, lower button)
15  [] Button        (0=Pressed, 1=Released) (Square, left button)
*/
union DigitalControllerSwitches {
    struct {
        uint16_t select : 1;
        uint16_t L3 : 1;
        uint16_t R3 : 1;
        uint16_t start : 1;
        uint16_t up : 1;
        uint16_t right : 1;
        uint16_t down : 1;
        uint16_t left : 1;
        uint16_t L2 : 1;
        uint16_t R2 : 1;
        uint16_t L1 : 1;
        uint16_t R1 : 1;
        uint16_t triangle : 1;
        uint16_t circle : 1;
        uint16_t x : 1;
        uint16_t square : 1;
    };

    uint16_t _value;

    DigitalControllerSwitches() : _value(0xFFFF) {}

    void reset() {
        _value = 0xFFFF;
    }
};

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
    TransferStopped = 5,
};

class DigitalController {
    Logger logger;
    SDL_Joystick *joystick;
    CommunicationSequenceStage currentStage;
    uint16_t identifier;
    DigitalControllerSwitches switches;

    void updateWithJoystick();
    void updateWithKeyboard();
public:
    DigitalController(LogLevel logLevel);
    ~DigitalController();

    uint8_t getResponse(uint8_t value);
    CommunicationSequenceStage getCurrentStage();
    bool getAcknowledge();
    void updateInput();
};
