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

void GTEFlagRegister::setRGB(unsigned int index) {
    switch (index) {
        case 1: {
            colorFifoR = 1;
            break;
        }
        case 2: {
            colorFifoG = 1;
            break;
        }
        case 3: {
            colorFifoB = 1;
            break;
        }
    }
    return;
}

void GTEFlagRegister::setSXY2(unsigned int index) {
    switch (index) {
        case 1: {
            sx2 = 1;
            break;
        }
        case 2: {
            sy2 = 1;
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

int16_t GTEFlagRegister::calculateIR0(int64_t value) {
    if (value < 0) {
        ir0 = 1;
        return 0;
    }

    if (value > 0x1000) {
        ir0 = 1;
        return 0x1000;
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

uint8_t GTEFlagRegister::calculateRGB(unsigned int index, int value) {
    if (value < 0) {
        setRGB(index);
        return 0;
    }
    if (value > 0xFF) {
        setRGB(index);
        return 0xFF;
    }

    return value;
}

int16_t GTEFlagRegister::calculateSXY2(unsigned int index, int value) {
    if (value < -0x400) {
        setSXY2(index);
        return -0x400;
    }
    if (value > 0x3FF) {
        setSXY2(index);
        return 0x3FF;
    }

    return value;
}
