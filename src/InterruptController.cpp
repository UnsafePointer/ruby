#include "InterruptController.hpp"

InterruptController::InterruptController() : status(), mask() {

}

InterruptController::~InterruptController() {

}

void InterruptController::setStatus(uint16_t status) {
    this->status &= status;
}

void InterruptController::setMask(uint16_t mask) {
    this->mask = mask;
}
