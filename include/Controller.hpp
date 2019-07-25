#pragma once
#include <cstdint>

class Controller {

public:
    Controller();
    ~Controller();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);
};
