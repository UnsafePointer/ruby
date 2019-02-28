#pragma once
#include "BIOS.hpp"
#include "RAM.hpp"
#include "DMA.hpp"
#include "GPU.hpp"

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
    const BIOS &bios;
    RAM &ram;
    DMA &dma;
    GPU &gpu;
    uint32_t maskRegion(uint32_t address) const;
    uint32_t dmaRegister(uint32_t offset) const;
    void setDMARegister(uint32_t offset, uint32_t value) const;
public:
    Interconnect(BIOS &bios, RAM &ram, DMA &dma, GPU &gpu);
    ~Interconnect();

    uint32_t loadWord(uint32_t address) const;
    uint16_t loadHalfWord(uint32_t address) const;
    uint8_t loadByte(uint32_t address) const;
    void storeWord(uint32_t address, uint32_t value) const;
    void storeHalfWord(uint32_t address, uint16_t value) const;
    void storeByte(uint32_t address, uint8_t value) const;
};
