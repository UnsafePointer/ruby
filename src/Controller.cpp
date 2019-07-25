#include "Controller.hpp"

Controller::Controller() : control(), joypadBaud() {

}

Controller::~Controller() {

}

void Controller::setControlRegister(uint16_t value) {
    control._value = value;
}

void Controller::setJoypadBaudRegister(uint16_t value) {
    joypadBaud = value;
}
