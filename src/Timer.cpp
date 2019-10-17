#include "Timer.hpp"
#include "Constants.h"
#include "Output.hpp"

Timer::Timer(uint8_t identity) : logger(LogLevel::NoLog), identity(identity), counterValue(), counterMode(), counterTarget(), counter() {}

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
    Timer0ClockSource clockSource = counterMode.timer0ClockSource();
    if (clockSource == Timer0ClockSource::DotClock) {
        uint32_t videoCycles = cycles*11/7;
        counter += videoCycles;
        counterValue.value += counter / VideoSystemClocksPerDot;
        counter %= VideoSystemClocksPerDot;
    } else {
        counter += cycles;
        counterValue.value += cycles;
    }

    checkTargetsAndOverflows();
}

void Timer0::setCounterModeRegister(uint32_t value) {
    counterMode._value = value;
    counterValue._value = 0;
}

void Timer1::step(uint32_t cycles) {
    Timer1ClockSource clockSource = counterMode.timer1ClockSource();
    if (clockSource == Timer1ClockSource::Hblank) {
        uint32_t videoCycles = cycles*11/7;
        counter += videoCycles;
        counterValue.value += counter / VideoSystemClocksPerScanline;
        counter %= VideoSystemClocksPerScanline;
    } else {
        counter += cycles;
        counterValue.value += cycles;
    }

    checkTargetsAndOverflows();
}

void Timer1::setCounterModeRegister(uint32_t value) {
    counterMode._value = value;
    counterValue._value = 0;
}

void Timer2::step(uint32_t cycles) {
    Timer2ClockSource clockSource = counterMode.timer2ClockSource();
    if (clockSource == Timer2ClockSource::SystemClockByEight) {
        counter += cycles;
        counterValue.value += counter / 8;
        counter %= 8;
    } else {
        counter += cycles;
        counterValue.value += cycles;
    }

    checkTargetsAndOverflows();
}

void Timer2::setCounterModeRegister(uint32_t value) {
    counterMode._value = value;
    counterValue._value = 0;
}

void Timer::checkTargetsAndOverflows() {
    bool checkIRQ = false;

    if (counterValue.value >= counterTarget.target) {
        counterMode.rearchedTarget = true;
        TimerResetCounter timerResetCounter = counterMode.timerResetCounter();
        if (timerResetCounter == AfterTarget) {
            counterValue._value = 0;
        }
        if (counterMode.IRQWhenTarget) {
            checkIRQ = true;
        }
    }

    if (counterValue.value >= 0xffff) {
        counterMode.rearchedOverflow = true;
        TimerResetCounter timerResetCounter = counterMode.timerResetCounter();
        if (timerResetCounter == AfterOverflow) {
            counterValue._value = 0;
        }
        if (counterMode.IRQWhenOverflow) {
            checkIRQ = true;
        }
    }

    if (!checkIRQ) {
        return;
    }

    checkInterruptRequest();
}

void Timer::checkInterruptRequest() {
    logger.logWarning(format("Unhandled interrupt trigger for timer %d", identity));
}
