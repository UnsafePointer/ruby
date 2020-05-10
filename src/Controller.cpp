#include "Controller.hpp"

using namespace std;

/*
IRQ7 is usually triggered circa 1500 cycles after sending a byte (counted from the begin of
the first bit), except, the last byte doesn't trigger IRQ7, and, after the 7th byte of the
Read command, an additional delay of circa 31000 cycles occurs before IRQ7 gets triggered
(that strange extra delay occurs only on original Sony cards, not on cards from other
manufacturers).
*/
const uint32_t SystemClocksPerControllerInt7 = 1500;

Controller::Controller(LogLevel logLevel, std::unique_ptr<InterruptController> &interruptController) : logger(logLevel, "  CONTROLLER: "), interruptController(interruptController), digitalController(make_unique<DigitalController>(logLevel)), currentDevice(NoDevice), control(), joypadBaud(), mode(), rxData(), status(), txData() {

}

Controller::~Controller() {

}

void Controller::setControlRegister(uint16_t value) {
    logger.logMessage("JOY_CTRL [W]: %#x", value);
    control._value = value;
    if (control.acknowledge) {
        status.interruptRequest = false;
    }
}

void Controller::setJoypadBaudRegister(uint16_t value) {
    logger.logMessage("JOY_BAUD [W]: %#x", value);
    joypadBaud = value;
}

void Controller::setModeRegister(uint16_t value) {
    logger.logMessage("JOY_MODE [W]: %#x", value);
    mode._value = value;
}

void Controller::setTxDataRegister(uint8_t value) {
    logger.logMessage("JOY_TX_DATA [W]: %#x", value);
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
        currentDevice = NoDevice;
        return;
    } else if (currentDevice == Device::ControllerDevice) {
        if (control.desiredSlotNumber == 1) {
            // TODO: Implement controller 2
            rxData.receivedData = 0xff;
            currentDevice = NoDevice;
            return;
        } else {
            rxData.receivedData = digitalController->getResponse(value);;
            control.acknowledge = digitalController->getAcknowledge();
            status.ackInputLevel = true;
            if (control.acknowledge) {
                shouldCount = true;
                counter = 0;
            }
            if (digitalController->getCurrentStage() == CommunicationSequenceStage::ControllerAccess) {
                currentDevice = NoDevice;
            }
        }
    }
}

uint8_t Controller::getRxDataRegister() {
    logger.logMessage("JOY_RX_DATA [R]: %#x", rxData._value);
    status.rxFifoNotEmpty = false;
    return rxData.receivedData;
}

uint32_t Controller::getStatusRegister() {
    logger.logMessage("JOY_STAT [R]: %#x", status._value);
    status.txReadyFlag1 = true;
    status.txReadyFlag2 = true;
    uint32_t value = status._value;
    status.ackInputLevel = false;
    return value;
}

uint16_t Controller::getControlRegister() {
    logger.logMessage("JOY_CTRL [R]: %#x", control._value);
    return control._value;
}

void Controller::step(uint32_t steps) {
    if (shouldCount) {
        counter += steps;
        if (counter >= SystemClocksPerControllerInt7) {
            status.interruptRequest = true;
            status.ackInputLevel = false;
            shouldCount = false;
            counter = 0;
        }
    }
    if (status.interruptRequest) {
        interruptController->trigger(InterruptRequestNumber::CONTROLLER);
    }
}

void Controller::updateInput(SDL_Event event) {
    digitalController->updateInput(event);
}
