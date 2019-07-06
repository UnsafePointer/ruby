#pragma once
#include <memory>
#include "CPU.hpp"
#include "COP0.hpp"
#include "Interconnect.hpp"
#include "BIOS.hpp"
#include "RAM.hpp"
#include "GPU.hpp"
#include "DMA.hpp"
#include "Scratchpad.hpp"
#include "CDROM.hpp"
#include "InterruptController.hpp"
#include "Expansion1.hpp"

class Emulator {
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<COP0> cop0;
    std::unique_ptr<Interconnect> interconnect;
    std::unique_ptr<BIOS> bios;
    std::unique_ptr<RAM> ram;
    std::unique_ptr<GPU> gpu;
    std::unique_ptr<DMA> dma;
    std::unique_ptr<Scratchpad> scratchpad;
    std::unique_ptr<CDROM> cdrom;
    std::unique_ptr<InterruptController> interruptController;
    std::unique_ptr<Expansion1> expansion1;
public:
    Emulator();
    ~Emulator();

    CPU* getCPU();
    void emulateFrame();
};
