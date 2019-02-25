#pragma once
#include <cstdint>

enum Direction {
    ToRam = 0,
    FromRam = 1
};

enum Step {
    Increment = 0,
    Decrement = 1
};

enum Sync {
    Manual = 0,
    Request = 1,
    LinkedList = 2
};

// 1F801088h+N*10h - D#_CHCR - DMA Channel Control (Channel 0..6) (R/W)
// 0       Transfer Direction    (0=To Main RAM, 1=From Main RAM)
// 1       Memory Address Step   (0=Forward;+4, 1=Backward;-4)
// 2-7     Not used              (always zero)
// 8       Chopping Enable       (0=Normal, 1=Chopping; run CPU during DMA gaps)
// 9-10    SyncMode, Transfer Synchronisation/Mode (0-3):
//         0  Start immediately and transfer all at once (used for CDROM, OTC)
//         1  Sync blocks to DMA requests   (used for MDEC, SPU, and GPU-data)
//         2  Linked-List mode              (used for GPU-command-lists)
//         3  Reserved                      (not used)
// 11-15   Not used              (always zero)
// 16-18   Chopping DMA Window Size (1 SHL N words)
// 19      Not used              (always zero)
// 20-22   Chopping CPU Window Size (1 SHL N clks)
// 23      Not used              (always zero)
// 24      Start/Busy            (0=Stopped/Completed, 1=Start/Enable/Busy)
// 25-27   Not used              (always zero)
// 28      Start/Trigger         (0=Normal, 1=Manual Start; use for SyncMode=0)
// 29      Unknown (R/W) Pause?  (0=No, 1=Pause?)     (For SyncMode=0 only?)
// 30      Unknown (R/W)
// 31      Not used              (always zero)
class Channel {
    bool enable;
    Direction direction;
    Step step;
    Sync sync;
    bool trigger;
    bool chop;
    uint8_t chopDMAWindowSize;
    uint8_t chopCPUWindowSize;
    uint8_t unknown;

    uint32_t baseAddress;
    uint16_t blockSize;
    uint16_t blockCount;
public:
    Channel();
    ~Channel();

    uint32_t controlRegister() const;
    void setControlRegister(uint32_t value);
    uint32_t baseAddressRegister() const;
    void setBaseAddressRegister(uint32_t value);
    uint32_t blockControlRegister() const;
    void setBlockControlRegister(uint32_t value);
};
