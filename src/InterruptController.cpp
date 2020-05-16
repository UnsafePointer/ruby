#include "InterruptController.hpp"

using namespace std;

InterruptController::InterruptController(LogLevel logLevel) : logger(logLevel, "  IRQ: "), status(), mask() {

}

InterruptController::~InterruptController() {

}

string InterruptController::requestNumberDescription(InterruptRequestNumber requestNumber) const {
    switch (requestNumber) {
        case VBLANK:
            return "VBLANK";
        case GPUIRQ:
            return "GPU";
        case CDROMIRQ:
            return "CDROM";
        case DMAIRQ:
            return "DMA";
        case TIMER0:
            return "TIMER0";
        case TIMER1:
            return "TIMER1";
        case TIMER2:
            return "TIMER2";
        case CONTROLLER:
            return "CONTROLLER";
        case SIO:
            return "SIO";
        case SPUIRQ:
            return "SPU";
        case LIGHTPEN:
            return "LIGHTPEN";
        default:
            return "none";
    }
}

void InterruptController::trigger(InterruptRequestNumber irq) {
    logger.logMessage("%s interrupt request enabled", requestNumberDescription(irq).c_str());
    status.value |= (1 << irq);
}

bool InterruptController::areInterruptsPending() {
    return (status.value & mask.value) != 0;
}

void InterruptController::setStatus(uint16_t status) {
    this->status.value &= status & 0x7FF;
}

void InterruptController::setMask(uint16_t mask) {
    this->mask.value = mask & 0x7FF;
}
