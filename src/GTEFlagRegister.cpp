#include "GTEFlagRegister.hpp"

void GTEFlagRegister::setMACNegative(unsigned int index) {
    switch (index) {
        case 1: {
            mac1Negative = 1;
            break;
        }
        case 2: {
            mac2Negative = 1;
            break;
        }
        case 3: {
            mac3Negative = 1;
            break;
        }
    }
    return;
}

void GTEFlagRegister::setMACPositive(unsigned int index) {
    switch (index) {
        case 1: {
            mac1Positive = 1;
            break;
        }
        case 2: {
            mac2Positive = 1;
            break;
        }
        case 3: {
            mac3Positive = 1;
            break;
        }
    }
    return;
}

void GTEFlagRegister::setIR(unsigned int index) {
    switch (index) {
        case 1: {
            ir1 = 1;
            break;
        }
        case 2: {
            ir2 = 1;
            break;
        }
        case 3: {
            ir3 = 1;
            break;
        }
    }
    return;
}

int64_t GTEFlagRegister::calculateMAC(unsigned int index, int64_t value) {
    if (value < -0x80000000000) {
        setMACNegative(index);
    }
    if (value > 0x7FFFFFFFFFF) {
        setMACPositive(index);
    }

    return (value << 20) >> 20;
}

int32_t GTEFlagRegister::calculateMAC0(int64_t value) {
    if (value < -(int64_t)0x80000000) {
        mac0Negative = 1;
    }
    if (value > 0x7FFFFFFF) {
        mac0Positive = 1;
    }

    return value;
}

int16_t GTEFlagRegister::calculateIR(unsigned int index, int64_t value, bool lm) {
    if (lm && value < 0) {
        setIR(index);
        return 0;
    }
    if (!lm && value < -0x8000) {
        setIR(index);
        return -0x8000;
    }
    if (value > 0x7FFF) {
        setIR(index);
        return 0x7FFF;
    }

    return value;
}

uint16_t GTEFlagRegister::calculateSZ3(int64_t value) {
    if (value < 0) {
        sz3OrOtz = 1;
        return 0;
    }
    if (value > 0xFFFF) {
        sz3OrOtz = 1;
        return 0xFFFF;
    }

    return value;
}
