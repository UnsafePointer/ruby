#pragma once
#include <cstdint>
#include <memory>
#include "Channel.hpp"
#include "RAM.hpp"
#include "GPU.hpp"

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

enum Port {
    MDECin = 0,
    MDECout = 1,
    GPUP = 2,
    CDROM = 3,
    SPU = 4,
    PIO = 5,
    OTC = 6,
    None = 1337
};

Port portWithIndex(uint32_t index);

class DMA {
    std::unique_ptr<RAM> &ram;
    std::unique_ptr<GPU> &gpu;

    uint32_t controlRegister;

// 1F8010F4h - DICR - DMA Interrupt Register (R/W)
// 0-5   Unknown  (read/write-able)
// 6-14  Not used (always zero)
// 15    Force IRQ (sets bit31)            (0=None, 1=Force Bit31=1)
// 16-22 IRQ Enable for DMA0..DMA6         (0=None, 1=Enable)
// 23    IRQ Master Enable for DMA0..DMA6  (0=None, 1=Enable)
// 24-30 IRQ Flags for DMA0..DMA6          (0=None, 1=IRQ)    (Write 1 to reset)
// 31    IRQ Master Flag                   (0=None, 1=IRQ)    (Read only)
    uint8_t interruptRequestUnknown;
    bool forceInterruptRequest;
    uint8_t interruptRequestChannelEnable;
    bool interruptRequestEnable;
    uint8_t interruptRequestChannelFlags;

    Channel channels[7];

    bool interruptRequestStatus() const;
    void executeBlock(Channel channel);
public:
    DMA(std::unique_ptr<RAM> &ram, std::unique_ptr<GPU> &gpu);
    ~DMA();

    uint32_t ctrlRegister() const;
    void setControlRegister(uint32_t value);
    uint32_t interruptRegister() const;
    void setInterruptRegister(uint32_t value);
    Channel& channelForPort(Port port);

    void execute(Port port);
    void executeLinkedList(Port port, Channel& channel);
    void executeBlock(Port port, Channel& channel);
};
