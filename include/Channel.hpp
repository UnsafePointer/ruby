#pragma once
#include <cstdint>
#include <optional>

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
union ChannelControl {
    struct {
        uint32_t _direction : 1;
        uint32_t _step : 1;
        uint32_t : 6;
        uint32_t chopping : 1;
        uint32_t _sync : 2;
        uint32_t : 5;
        uint32_t choppingDMAWindowSize : 3;
        uint32_t : 1;
        uint32_t choppingCPUWindowSize : 3;
        uint32_t : 1;
        uint32_t enable : 1;
        uint32_t : 3;
        uint32_t trigger : 1;
        uint32_t pause : 1;
        uint32_t unknown : 1;
        uint32_t : 1;
    };
    uint32_t value;

    ChannelControl() : value(0) {}

    Sync sync() const { return Sync(_sync); }
    Direction direction() const { return Direction(_direction); }
    Step step() const { return Step(_step); }
};

// 1F801080h+N*10h - D#_MADR - DMA base address (Channel 0..6) (R/W)
// 0-23  Memory Address where the DMA will start reading from/writing to
// 24-31 Not used (always zero)
union ChannelBaseAddress {
    struct {
        uint32_t address : 24;
        uint32_t unknown : 8;
    };
    uint32_t value;

    ChannelBaseAddress() : value(0) {}
};

// 1F801084h+N*10h - D#_BCR - DMA Block Control (Channel 0..6) (R/W)
// For SyncMode=0 (ie. for OTC and CDROM):
//   0-15  BC    Number of words (0001h..FFFFh) (or 0=10000h words)
//   16-31 0     Not used (usually 0 for OTC, or 1 ("one block") for CDROM)
// For SyncMode=1 (ie. for MDEC, SPU, and GPU-vram-data):
//   0-15  BS    Blocksize (words) ;for GPU/SPU max 10h, for MDEC max 20h
//   16-31 BA    Amount of blocks  ;ie. total length = BS*BA words
// For SyncMode=2 (ie. for GPU-command-lists):
//   0-31  0     Not used (should be zero) (transfer ends at END-CODE in list)
union ChannelBlockControl {
    struct {
        uint32_t blockSize : 16;
        uint32_t blockCount : 16;
    };
    uint32_t value;

    ChannelBlockControl() : value(0) {}
};

class Channel {
    ChannelControl control;
    ChannelBaseAddress baseAddress;
    ChannelBlockControl blockControl;
public:
    Channel();
    ~Channel();

    uint32_t controlRegister() const;
    void setControlRegister(uint32_t value);
    uint32_t baseAddressRegister() const;
    void setBaseAddressRegister(uint32_t value);
    uint32_t blockControlRegister() const;
    void setBlockControlRegister(uint32_t value);
    bool isActive() const;
    Sync sync() const;
    Direction direction() const;
    Step step() const;
    std::optional<uint32_t> transferSize() const;
    void done();
};
