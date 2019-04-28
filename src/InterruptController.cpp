#include "InterruptController.hpp"

InterruptController::InterruptController() : status(), mask() {

}

InterruptController::~InterruptController() {

}

bool InterruptController::isActive() {
    return (status & mask) != 0;
}

void InterruptController::setStatus(uint16_t status) {
    this->status &= status;
}

void InterruptController::setMask(uint16_t mask) {
    this->mask = mask;
}
