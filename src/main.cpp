#include "BIOS.hpp"
#include "Interconnect.hpp"
#include "CPU.hpp"
#include "DMA.hpp"
#include "GPU.hpp"

int main() {
    BIOS bios = BIOS();
    bios.loadBin("SCPH1001.BIN");
    RAM ram = RAM();
    DMA dma = DMA(ram);
    GPU gpu = GPU();
    Interconnect interconnect = Interconnect(bios, ram, dma, gpu);
    CPU cpu = CPU(interconnect);
    while (true) {
        cpu.executeNextInstruction();
    }
}
