#include "Controller.hpp"

Controller::Controller(LogLevel logLevel) : logger(logLevel), control(), joypadBaud(), mode(), rxData(), status(), txData() {

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

void Controller::setTxDataRegister(uint8_t value) {
    status.rxFifoNotEmpty = true;
    txData._value = value;
    // TODO: send data to device
    rxData.receivedData = 0xff;
    status.ackInputLevel = true;
}

uint8_t Controller::getRxDataRegister() {
    status.rxFifoNotEmpty = false;
    return rxData.receivedData;
}

uint32_t Controller::getStatusRegister() {
    status.txReadyFlag1 = true;
    status.txReadyFlag1 = true;
    uint32_t value = status._value;
    status.ackInputLevel = false;
    return value;
}

uint16_t Controller::getControlRegister() {
    return control._value;
}
