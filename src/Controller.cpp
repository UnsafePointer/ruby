#include "Controller.hpp"

Controller::Controller() : control(), joypadBaud(), mode(), rxData(), status() {

}

Controller::~Controller() {

}

void Controller::setControlRegister(uint16_t value) {
    control._value = value;
}

void Controller::setJoypadBaudRegister(uint16_t value) {
    joypadBaud = value;
}

void Controller::setModeRegister(uint16_t value) {
    mode._value = value;
}

uint8_t Controller::getRxDataRegister() const {
    return rxData.receivedData;
}

uint32_t Controller::getStatusRegister() const {
    return status._value;
}
