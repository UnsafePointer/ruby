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

void DigitalController::updateWithJoystick() {
    if (SDL_JoystickGetButton(joystick, 0) != 0) { // TRIANGLE
        switches.triangle = false;
    }
    if (SDL_JoystickGetButton(joystick, 1) != 0) { // CIRCLE
        switches.circle = false;
    }
    if (SDL_JoystickGetButton(joystick, 2) != 0) { // X
        switches.x = false;
    }
    if (SDL_JoystickGetButton(joystick, 3) != 0) { // SQUARE
        switches.square = false;
    }
    if (SDL_JoystickGetButton(joystick, 4) != 0) { // L2
        switches.L2 = false;
    }
    if (SDL_JoystickGetButton(joystick, 5) != 0) { // R2
        switches.R2 = false;
    }
    if (SDL_JoystickGetButton(joystick, 6) != 0) { // L1
        switches.L1 = false;
    }
    if (SDL_JoystickGetButton(joystick, 7) != 0) { // R1
        switches.R1 = false;
    }
    if (SDL_JoystickGetButton(joystick, 8) != 0) { // SELECT
        switches.select = false;
    }
    if (SDL_JoystickGetButton(joystick, 9) != 0) { // START
        switches.start = false;
    }
    Sint16 X = SDL_JoystickGetAxis(joystick, 0); // LEFT OR RIGHT
    if (X != 0) {
        if (X == std::numeric_limits<int16_t>::max()) {
            switches.right = false;
        } else {
            switches.left = false;
        }
    }
    Sint16 Y = SDL_JoystickGetAxis(joystick, 1); // UP OR DOWN
    if (Y != 0) {
        if (Y == std::numeric_limits<int16_t>::max()) {
            switches.down = false;
        } else {
            switches.up = false;
        }
    }
}

void DigitalController::updateWithKeyboard() {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_W]) { // TRIANGLE
        switches.triangle = false;
    }
    if (state[SDL_SCANCODE_D]) { // CIRCLE
        switches.circle = false;
    }
    if (state[SDL_SCANCODE_S]) { // X
        switches.x = false;
    }
    if (state[SDL_SCANCODE_A]) { // SQUARE
        switches.square = false;
    }
    if (state[SDL_SCANCODE_1]) { // L2
        switches.L2 = false;
    }
    if (state[SDL_SCANCODE_3]) { // R2
        switches.R2 = false;
    }
    if (state[SDL_SCANCODE_Q]) { // L1
        switches.L1 = false;
    }
    if (state[SDL_SCANCODE_E]) { // R1
        switches.R1 = false;
    }
    if (state[SDL_SCANCODE_SPACE]) { // SELECT
        switches.select = false;
    }
    if (state[SDL_SCANCODE_KP_ENTER]) { // START
        switches.start = false;
    }
    if (state[SDL_SCANCODE_LEFT]) { // LEFT
        switches.left = false;
    }
    if (state[SDL_SCANCODE_RIGHT]) { // RIGHT
        switches.right = false;
    }
    if (state[SDL_SCANCODE_UP]) { // UP
        switches.up = false;
    }
    if (state[SDL_SCANCODE_DOWN]) { // DOWN
        switches.down = false;
    }
}

void DigitalController::updateInput() {
    switches.reset();

    if (joystick == nullptr) {
        updateWithKeyboard();
    } else {
        updateWithJoystick();
    }
}
