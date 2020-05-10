#include "DigitalController.hpp"
#include "ConfigurationManager.hpp"
#include <string>
#include <limits>

using namespace std;

DigitalController::DigitalController(LogLevel logLevel) : logger(logLevel), joystick(nullptr), currentStage(CommunicationSequenceStage::ControllerAccess), identifier(0x5A41), switches() {
    ConfigurationManager *configurationManager = ConfigurationManager::getInstance();
    string controllerName = configurationManager->controllerName();
    int numberOfJoysticks = SDL_NumJoysticks();
    if (numberOfJoysticks < 0) {
        logger.logError("Error getting number of joysticks: %s", SDL_GetError());
    }
    for(int i = 0; i < numberOfJoysticks; i++) {
        SDL_Joystick *currentJoystick = SDL_JoystickOpen(i);
        std::string joystickName = SDL_JoystickName(currentJoystick);
        if (joystickName.compare(controllerName) == 0) {
            joystick = currentJoystick;
            break;
        }
    }
    if (joystick == nullptr) {
        logger.logWarning("Failed to find target controller. Defaulting to keyboard bindings.");
        return;
    }
    int numberOfButtons = SDL_JoystickNumButtons(joystick);
    logger.logMessage("Buttons in joystick: %d buttons", numberOfButtons);
    if (numberOfButtons < 0) {
        logger.logError("Error getting number of buttons: %s", SDL_GetError());
    }
    int numberOfAxes = SDL_JoystickNumAxes(joystick);
    logger.logMessage("Axes in joystick: %d buttons", numberOfAxes);
    if (numberOfAxes < 0) {
        logger.logError("Error getting number of axes: %s", SDL_GetError());
    }
}

DigitalController::~DigitalController() {

}

uint8_t DigitalController::getResponse(uint8_t value) {
    switch (currentStage) {
        case ControllerAccess: {
            if (value == 0x1) {
                currentStage = ReceiveIDLow;
            }
            return 0xff; // Hi-Z
        }
        case ReceiveIDLow: {
            if (value == 0x42) {
                currentStage = ReceiveIDHigh;
                uint8_t response = identifier & 0xFF;
                return response;
            }
            currentStage = ControllerAccess;
            return 0xff;
        }
        case ReceiveIDHigh: {
            // TODO: Handle TAP validation?
            currentStage = ReceiveDigitalSwitchesLow;
            uint8_t response = (identifier & 0xFF00) >> 8;
            return response;
        }
        case ReceiveDigitalSwitchesLow: {
            // TODO: Handle MOT validation?
            currentStage = ReceiveDigitalSwitchesHigh;
            return switches._value & 0xFF;
        }
        case ReceiveDigitalSwitchesHigh: {
            // TODO: Handle MOT validation?
            currentStage = TransferStopped;
            return switches._value >> 8;
        }
        case TransferStopped: {
            currentStage = ControllerAccess;
            return 0xff;
        }
    }
    return 0x0;
}

CommunicationSequenceStage DigitalController::getCurrentStage() {
    return currentStage;
}

bool DigitalController::getAcknowledge() {
    return currentStage != ControllerAccess;
}

void DigitalController::updateInput(SDL_Event event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_w: { // TRIANGLE
                switches.triangle = false;
                break;
            }
            case SDLK_d: { // CIRCLE
                switches.circle = false;
                break;
            }
            case SDLK_s: { // X
                switches.x = false;
                break;
            }
            case SDLK_a: { // SQUARE
                switches.square = false;
                break;
            }
            case SDLK_1: { // L2
                switches.L2 = false;
                break;
            }
            case SDLK_3: { // R2
                switches.R2 = false;
                break;
            }
            case SDLK_q: { // R1
                switches.R1 = false;
                break;
            }
            case SDLK_e: { // L1
                switches.L1 = false;
                break;
            }
            case SDLK_SPACE: { // SELECT
                switches.select = false;
                break;
            }
            case SDLK_RETURN: { // START
                switches.start = false;
                break;
            }
            case SDLK_LEFT: { // LEFT
                switches.left = false;
                break;
            }
            case SDLK_RIGHT: { // RIGHT
                switches.right = false;
                break;
            }
            case SDLK_UP: { // UP
                switches.up = false;
                break;
            }
            case SDLK_DOWN: { // DOWN
                switches.down = false;
                break;
            }
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_w: { // TRIANGLE
                switches.triangle = true;
                break;
            }
            case SDLK_d: { // CIRCLE
                switches.circle = true;
                break;
            }
            case SDLK_s: { // X
                switches.x = true;
                break;
            }
            case SDLK_a: { // SQUARE
                switches.square = true;
                break;
            }
            case SDLK_1: { // L2
                switches.L2 = true;
                break;
            }
            case SDLK_3: { // R2
                switches.R2 = true;
                break;
            }
            case SDLK_q: { // R1
                switches.R1 = true;
                break;
            }
            case SDLK_e: { // L1
                switches.L1 = true;
                break;
            }
            case SDLK_SPACE: { // SELECT
                switches.select = true;
                break;
            }
            case SDLK_RETURN: { // START
                switches.start = true;
                break;
            }
            case SDLK_LEFT: { // LEFT
                switches.left = true;
                break;
            }
            case SDLK_RIGHT: { // RIGHT
                switches.right = true;
                break;
            }
            case SDLK_UP: { // UP
                switches.up = true;
                break;
            }
            case SDLK_DOWN: { // DOWN
                switches.down = true;
                break;
            }
        }
    }
}
