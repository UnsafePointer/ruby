#pragma once
#include "GPU.hpp"
#include <iostream>

template <typename T>
inline T GPU::load(uint32_t offset) const {
    switch (offset) {
        case 0: {
            return readRegister();
        }
        case 4: {
            return statusRegister();
        }
        default: {
            std::cout << "Unhandled GPU read at offset: 0x" << std::hex << offset << std::endl;
            exit(1);
            return 0;
        }
    }
}
