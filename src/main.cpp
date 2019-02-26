#include "BIOS.hpp"
#include "Interconnect.hpp"
#include "CPU.hpp"
#include "DMA.hpp"

int main() {
    BIOS bios = BIOS();
    bios.loadBin("SCPH1001.BIN");
    RAM ram = RAM();
    DMA dma = DMA(ram);
    Interconnect interconnect = Interconnect(bios, ram, dma);
    CPU cpu = CPU(interconnect);
    while (true) {
        cpu.executeNextInstruction();
    }
}
