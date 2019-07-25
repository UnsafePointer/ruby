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

uint8_t Controller::getRxDataRegister() {
    return rxData.receivedData;
}

uint32_t Controller::getStatusRegister() {
    status.txReadyFlag1 = true;
    status.txReadyFlag1 = true;
    return status._value;
}
