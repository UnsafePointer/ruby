#pragma once
#include <cstdint>
#include <memory>
#include "Channel.hpp"
#include "RAM.hpp"
#include "GPU.hpp"
#include <string>
#include "CDROM.hpp"

enum Port {
    MDECin = 0,
    MDECout = 1,
    GPUP = 2,
    CDROMP = 3,
    SPU = 4,
    PIO = 5,
    OTC = 6,
    None = 1337
};

Port portWithIndex(uint32_t index);
std::string portDescription(Port port);

// 1F8010F0h - DPCR - DMA Control Register (R/W)
// 0-2   DMA0, MDECin  Priority      (0..7; 0=Highest, 7=Lowest)
// 3     DMA0, MDECin  Master Enable (0=Disable, 1=Enable)
// 4-6   DMA1, MDECout Priority      (0..7; 0=Highest, 7=Lowest)
// 7     DMA1, MDECout Master Enable (0=Disable, 1=Enable)
// 8-10  DMA2, GPU     Priority      (0..7; 0=Highest, 7=Lowest)
// 11    DMA2, GPU     Master Enable (0=Disable, 1=Enable)
// 12-14 DMA3, CDROM   Priority      (0..7; 0=Highest, 7=Lowest)
// 15    DMA3, CDROM   Master Enable (0=Disable, 1=Enable)
// 16-18 DMA4, SPU     Priority      (0..7; 0=Highest, 7=Lowest)
// 19    DMA4, SPU     Master Enable (0=Disable, 1=Enable)
// 20-22 DMA5, PIO     Priority      (0..7; 0=Highest, 7=Lowest)
// 23    DMA5, PIO     Master Enable (0=Disable, 1=Enable)
// 24-26 DMA6, OTC     Priority      (0..7; 0=Highest, 7=Lowest)
// 27    DMA6, OTC     Master Enable (0=Disable, 1=Enable)
// 28-30 Unknown, Priority Offset or so? (R/W)
// 31    Unknown, no effect? (R/W)
union DMAControl {
    struct {
        uint32_t MDECinPriority : 2;
        uint32_t MDECinEnable : 1;
        uint32_t MDECoutPriority : 2;
        uint32_t MDECoutEnable : 1;
        uint32_t GPUPriority : 2;
        uint32_t GPUEnable : 1;
        uint32_t CDROMPriority : 2;
        uint32_t CDROMEnable : 1;
        uint32_t SPUPriority : 2;
        uint32_t SPUEnable : 1;
        uint32_t PIOPriority : 2;
        uint32_t PIOEnable : 1;
        uint32_t OTCPriority : 2;
        uint32_t OTCEnable : 1;
        uint32_t unknown : 2;
        uint32_t unknown2 : 1;
    };

    uint32_t value;

    DMAControl() : value(0x07654321) {}
};

union IRQEnable {
    struct {
        uint8_t MDECinIRQEnable : 1;
        uint8_t MDECoutIRQEnable : 1;
        uint8_t GPUIRQEnable : 1;
        uint8_t CDROMIRQEnable : 1;
        uint8_t SPUIRQEnable : 1;
        uint8_t PIOIRQEnable : 1;
        uint8_t OTCIRQEnable : 1;
    };

    uint8_t value;

    IRQEnable(uint8_t value) : value(value) {}
};

union IRQFlags {
    struct {
        uint8_t MDECinIRQFlags : 1;
        uint8_t MDECoutIRQFlags : 1;
        uint8_t GPUIRQFlags : 1;
        uint8_t CDROMIRQFlags : 1;
        uint8_t SPUIRQFlags : 1;
        uint8_t PIOIRQFlags : 1;
        uint8_t OTCIRQFlags : 1;
    };

    uint8_t value;

    IRQFlags(uint8_t value) : value(value) {}
};

// 1F8010F4h - DICR - DMA Interrupt Register (R/W)
// 0-5   Unknown  (read/write-able)
// 6-14  Not used (always zero)
// 15    Force IRQ (sets bit31)            (0=None, 1=Force Bit31=1)
// 16-22 IRQ Enable for DMA0..DMA6         (0=None, 1=Enable)
// 23    IRQ Master Enable for DMA0..DMA6  (0=None, 1=Enable)
// 24-30 IRQ Flags for DMA0..DMA6          (0=None, 1=IRQ)    (Write 1 to reset)
// 31    IRQ Master Flag                   (0=None, 1=IRQ)    (Read only)
union DMAInterrupt {
    struct {
        uint32_t unknown : 6;
        uint32_t : 9;
        uint32_t forceIRQ : 1;
        uint32_t _IRQEnable : 7;
        uint32_t masterIRQEnable : 1;
        uint32_t _IRQFlags : 7;
        uint32_t masterIRQFlag : 1;
    };

    uint32_t value;

    DMAInterrupt() : value(0) {}

    IRQEnable IRQEnableStatus() const { return IRQEnable(_IRQEnable); }
    IRQFlags IRQFlagsStatus() const { return IRQFlags(_IRQEnable); }
};

// DMA Register Summary
// 1F80108xh DMA0 channel 0  MDECin  (RAM to MDEC)
// 1F80109xh DMA1 channel 1  MDECout (MDEC to RAM)
// 1F8010Axh DMA2 channel 2  GPU (lists + image data)
// 1F8010Bxh DMA3 channel 3  CDROM   (CDROM to RAM)
// 1F8010Cxh DMA4 channel 4  SPU
// 1F8010Dxh DMA5 channel 5  PIO (Expansion Port)
// 1F8010Exh DMA6 channel 6  OTC (reverse clear OT) (GPU related)
// 1F8010F0h DPCR - DMA Control register
// 1F8010F4h DICR - DMA Interrupt register
class DMA {
    std::unique_ptr<RAM> &ram;
    std::unique_ptr<GPU> &gpu;
    std::unique_ptr<CDROM> &cdrom;

    DMAControl control;
    DMAInterrupt interrupt;

    Channel channels[7];
    Channel& channelForPort(Port port);

    uint32_t controlRegister() const;
    void setControlRegister(uint32_t value);
    uint32_t interruptRegister() const;
    void setInterruptRegister(uint32_t value);

    bool interruptRequestStatus() const;

    void execute(Port port);
    void executeBlock(Port port, Channel& channel);
    void executeLinkedList(Port port, Channel& channel);
public:
    DMA(std::unique_ptr<RAM> &ram, std::unique_ptr<GPU> &gpu, std::unique_ptr<CDROM> &cdrom);
    ~DMA();

    template <typename T>
    inline T load(uint32_t offset);
    template <typename T>
    inline void store(uint32_t offset, T value);
};
