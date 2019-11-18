#include "DigitalController.hpp"
#include "ConfigurationManager.hpp"
#include <string>

using namespace std;

DigitalController::DigitalController(LogLevel logLevel) : logger(logLevel), currentStage(CommunicationSequenceStage::ControllerAccess), identifier(0x5A41) {
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
        logger.logError("Failed to find target controller.");
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
            // TODO: Read values from SDL_Joystick
            return 0xF;
        }
        case ReceiveDigitalSwitchesHigh: {
            // TODO: Handle MOT validation?
            currentStage = ControllerAccess;
            // TODO: Read values from SDL_Joystick
            return 0xF;
        }
    }
    return 0x0;
}

CommunicationSequenceStage DigitalController::getCurrentStage() {
    return currentStage;
}
