#pragma once
#include <cstdint>

class DMA {
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

    bool interruptRequestStatus() const;
public:
    DMA();
    ~DMA();

    uint32_t ctrlRegister() const;
    void setControlRegister(uint32_t value);
    uint32_t interruptRegister() const;
    void setInterruptRegister(uint32_t value);

};
