#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include "COP0.hpp"
#include "Logger.hpp"

enum InterruptRequestNumber : uint16_t {
    VBLANK = 0,
    GPUIRQ = 1,
    CDROMIRQ = 2,
    DMAIRQ = 3,
    TIMER0 = 4,
    TIMER1 = 5,
    TIMER2 = 6,
    CONTROLLER = 7,
    SIO = 8,
    SPUIRQ = 9,
    LIGHTPEN = 10,
};

union IRQ {
    struct {
        uint16_t vblank : 1;
        uint16_t gpu : 1;
        uint16_t cdrom : 1;
        uint16_t dma : 1;
        uint16_t timer0 : 1;
        uint16_t timer1 : 1;
        uint16_t timer2 : 1;
        uint16_t controller : 1;
        uint16_t sio : 1;
        uint16_t spu : 1;
        uint16_t lightpen : 1;
    };
    uint16_t value;

    IRQ() : value(0) {}
};

/*
1F801070h I_STAT - Interrupt status register (R=Status, W=Acknowledge)
1F801074h I_MASK - Interrupt mask register (R/W)
Status: Read I_STAT (0=No IRQ, 1=IRQ)
Acknowledge: Write I_STAT (0=Clear Bit, 1=No change)
Mask: Read/Write I_MASK (0=Disabled, 1=Enabled)
  0     IRQ0 VBLANK (PAL=50Hz, NTSC=60Hz)
  1     IRQ1 GPU   Can be requested via GP0(1Fh) command (rarely used)
  2     IRQ2 CDROM
  3     IRQ3 DMA
  4     IRQ4 TMR0  Timer 0 aka Root Counter 0 (Sysclk or Dotclk)
  5     IRQ5 TMR1  Timer 1 aka Root Counter 1 (Sysclk or H-blank)
  6     IRQ6 TMR2  Timer 2 aka Root Counter 2 (Sysclk or Sysclk/8)
  7     IRQ7 Controller and Memory Card - Byte Received Interrupt
  8     IRQ8 SIO
  9     IRQ9 SPU
  10    IRQ10 Controller - Lightpen Interrupt (reportedly also PIO...?)
  11-15 Not used (always zero)
  16-31 Garbage
*/
class InterruptController {
    Logger logger;

    IRQ status;
    IRQ mask;

    void setStatus(uint16_t status);
    void setMask(uint16_t mask);

    std::string requestNumberDescription(InterruptRequestNumber requestNumber) const;
public:
    InterruptController(LogLevel logLevel);
    ~InterruptController();

    void trigger(InterruptRequestNumber irq);
    bool areInterruptsPending();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);
};
