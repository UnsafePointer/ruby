#include "InterruptController.hpp"

using namespace std;

InterruptController::InterruptController(unique_ptr<COP0> &cop0) : cop0(cop0), status(), mask() {

}

InterruptController::~InterruptController() {

}

bool InterruptController::isActive() {
    return (status.value & mask.value) != 0;
}

void InterruptController::update() {
    cop0->cause.interruptPending = isActive() ? 4 : 0;
}

void InterruptController::setStatus(uint16_t status) {
    this->status.value &= status;

    update();
}

void InterruptController::setMask(uint16_t mask) {
    this->mask.value = mask;

    update();
}
