#include "Controller.hpp"

Controller::Controller() : control() {

}

Controller::~Controller() {

}

void Controller::setControlRegister(uint16_t value) {
    control._value = value;
}
