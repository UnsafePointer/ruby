#include "DigitalController.hpp"
#include "ConfigurationManager.hpp"
#include <string>

using namespace std;

DigitalController::DigitalController(LogLevel logLevel) : logger(logLevel) {
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
