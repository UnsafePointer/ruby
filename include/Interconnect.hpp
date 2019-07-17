#pragma once
#include <memory>
#include "COP0.hpp"
#include "BIOS.hpp"
#include "RAM.hpp"
#include "DMA.hpp"
#include "GPU.hpp"
#include "Range.hpp"
#include "Scratchpad.hpp"
#include "CDROM.hpp"
#include "InterruptController.hpp"
#include "COP0.hpp"
#include "Expansion1.hpp"
#include "Timer.hpp"

const Range ramRange = Range(0x00000000, RAM_SIZE);
const Range scratchpadRange = Range(0x1f800000, SCRATCHPAD_SIZE);
const Range biosRange = Range(0x1fc00000, 512 * 1024);
const Range memoryControlRange = Range(0x1f801000, 36);
const Range ramSizeRange = Range(0x1f801060, 4);
const Range cacheControlRange = Range(0xfffe0130, 4);
const Range soundProcessingUnitRange = Range(0x1f801c00, 640);
const Range expansion2Range = Range(0x1f802000, 66);
const Range expansion1Range = Range(0x1f000000, EXPANSION1_SIZE);
const Range interruptRequestControlRange = Range(0x1f801070, 8);
const Range timer0RegisterRange = Range(0x1f801100, 16);
const Range timer1RegisterRange = Range(0x1f801110, 16);
const Range timer2RegisterRange = Range(0x1f801120, 16);
const Range dmaRegisterRange = Range(0x1f801080, 0x80);
const Range gpuRegisterRange = Range(0x1f801810, 8);
const Range cdromRegisterRange = Range(0x1f801800, 4);

/*
Memory Map
KUSEG     KSEG0     KSEG1
00000000h 80000000h A0000000h  2048K  Main RAM (first 64K reserved for BIOS)
1F000000h 9F000000h BF000000h  8192K  Expansion Region 1 (ROM/RAM)
1F800000h 9F800000h    --      1K     Scratchpad (D-Cache used as Fast RAM)
1F801000h 9F801000h BF801000h  8K     I/O Ports
1F802000h 9F802000h BF802000h  8K     Expansion Region 2 (I/O Ports)
1FA00000h 9FA00000h BFA00000h  2048K  Expansion Region 3 (whatever purpose)
1FC00000h 9FC00000h BFC00000h  512K   BIOS ROM (Kernel) (4096K max)
    FFFE0000h (KSEG2)        0.5K   I/O Ports (Cache Control)

See IOMap.md for I/O register mapping
*/
class Interconnect {
    std::unique_ptr<COP0> &cop0;
    std::unique_ptr<BIOS> &bios;
    std::unique_ptr<RAM> &ram;
    std::unique_ptr<GPU> &gpu;
    std::unique_ptr<DMA> &dma;
    std::unique_ptr<Scratchpad> &scratchpad;
    std::unique_ptr<CDROM> &cdrom;
    std::unique_ptr<InterruptController> &interruptController;
    std::unique_ptr<Expansion1> &expansion1;
    std::unique_ptr<Timer0> &timer0;
    std::unique_ptr<Timer1> &timer1;
    std::unique_ptr<Timer2> &timer2;
    uint32_t maskRegion(uint32_t address) const;
public:
    Interconnect(std::unique_ptr<COP0> &cop0, std::unique_ptr<BIOS> &bios, std::unique_ptr<RAM> &ram, std::unique_ptr<GPU> &gpu, std::unique_ptr<DMA> &dma, std::unique_ptr<Scratchpad> &scratchpad, std::unique_ptr<CDROM> &cdrom, std::unique_ptr<InterruptController> &interruptController, std::unique_ptr<Expansion1> &expansion1, std::unique_ptr<Timer0> &timer0, std::unique_ptr<Timer1> &timer1, std::unique_ptr<Timer2> &timer2);
    ~Interconnect();

    template <typename T>
    inline T load(uint32_t address) const;
    template <typename T>
    inline void store(uint32_t address, T value) const;

    void transferToRAM(std::string path, uint32_t origin, uint32_t size, uint32_t destination);
    void dumpRAM();
};
