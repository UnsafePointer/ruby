#include "Controller.hpp"

using namespace std;

Controller::Controller(LogLevel logLevel) : logger(logLevel), digitalController(make_unique<DigitalController>(logLevel)), currentDevice(NoDevice), control(), joypadBaud(), mode(), rxData(), status(), txData() {

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
    txData._value = value;
    status.rxFifoNotEmpty = true;

    if (currentDevice == NoDevice) {
        if (value == Device::ControllerDevice) {
            currentDevice = Device::ControllerDevice;
        } else if (value == Device::MemoryCardDevice) {
            currentDevice = Device::MemoryCardDevice;
        }
    }

    if (currentDevice == Device::MemoryCardDevice) {
        // TODO: Implement memory card
        rxData.receivedData = 0xff;
        return;
    } else if (currentDevice == Device::ControllerDevice) {
        if (control.desiredSlotNumber == 1) {
            // TODO: Implement controller 2
            rxData.receivedData = 0xff;
            return;
        } else {
            rxData.receivedData = digitalController->getResponse(value);
            if (digitalController->getCurrentStage() == CommunicationSequenceStage::ControllerAccess) {
                currentDevice = NoDevice;
            }
        }
    }
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
