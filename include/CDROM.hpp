#pragma once
#include <cstdint>
#include <queue>

/*
1F801800h - Index/Status Register (Bit0-1 R/W) (Bit2-7 Read Only)
0-1 Index   Port 1F801801h-1F801803h index (0..3 = Index0..Index3)   (R/W)
2   ADPBUSY XA-ADPCM fifo empty  (0=Empty) ;set when playing XA-ADPCM sound
3   PRMEMPT Parameter fifo empty (1=Empty) ;triggered before writing 1st byte
4   PRMWRDY Parameter fifo full  (0=Full)  ;triggered after writing 16 bytes
5   RSLRRDY Response fifo empty  (0=Empty) ;triggered after reading LAST byte
6   DRQSTS  Data fifo empty      (0=Empty) ;triggered after reading LAST byte
7   BUSYSTS Command/parameter transmission busy  (1=Busy)
*/
union CDROMStatus {
    struct {
        uint8_t index : 2;
        uint8_t XAADCPMFifoEmpty : 1;
        uint8_t parameterFifoEmpty : 1;
        uint8_t parameterFifoFull : 1;
        uint8_t responseFifoEmpty : 1;
        uint8_t dataFifoEmpty : 1;
        uint8_t transmissionBusy : 1;
    };

    uint8_t _value;

    CDROMStatus() : _value(0x18) {}
};

/*
1F801803h.Index0 - Interrupt Enable Register (R)
1F801803h.Index2 - Interrupt Enable Register (R) (Mirror)
0-4  Interrupt Enable Bits (usually all set, ie. 1Fh=Enable All IRQs)
5-7  Unknown/unused (write: should be zero) (read: usually all bits set)
*/
union CDROMInterrupt {
    struct {
        uint8_t enable : 4;
        uint8_t unknown : 4;
    };

    uint8_t _value;

    CDROMInterrupt() : _value(0) {}
};

class CDROM {
    CDROMStatus status;
    CDROMInterrupt interrupt;

    std::queue<uint8_t> parameters;

    void setStatusRegister(uint8_t value);
    void setInterruptRegister(uint8_t value);
    void setInterruptFlagRegister(uint8_t value);

    uint8_t getStatusRegister() const;

    void clearParameters();
    void pushParameter(uint8_t value);

    void updateStatusRegister();
public:
    CDROM();
    ~CDROM();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);
};
