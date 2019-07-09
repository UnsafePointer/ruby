#include "Timer.hpp"

Timer::Timer() : counterValue(), counterMode(), counterTarget() {}

Timer::~Timer() {}

uint32_t Timer::counterValueRegister() const {
    return counterValue._value;
}

uint32_t Timer::counterModeRegister() const {
    return counterMode._value;
}

uint32_t Timer::counterTargetRegister() const {
    return counterTarget._value;
}

void Timer::setCounterValueRegister(uint32_t value) {
    counterValue._value = value;
}

void Timer::setCounterTargetRegister(uint32_t value) {
    counterTarget._value = value;
}

void Timer0::step(uint32_t cycles) {
}

void Timer0::setCounterModeRegister(uint32_t value) {
    counterMode._value = value;
}

void Timer1::step(uint32_t cycles) {
}

void Timer1::setCounterModeRegister(uint32_t value) {
    counterMode._value = value;
    counterValue._value = 0;
}

void Timer2::step(uint32_t cycles) {
}

void Timer2::setCounterModeRegister(uint32_t value) {
    counterMode._value = value;
}
