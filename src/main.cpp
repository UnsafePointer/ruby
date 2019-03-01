#include "BIOS.hpp"
#include "Interconnect.hpp"
#include "CPU.hpp"
#include "DMA.hpp"
#include "GPU.hpp"

int main() {
    BIOS bios = BIOS();
    bios.loadBin("SCPH1001.BIN");
    RAM ram = RAM();
    GPU gpu = GPU();
    DMA dma = DMA(ram, gpu);
    Interconnect interconnect = Interconnect(bios, ram, dma, gpu);
    CPU cpu = CPU(interconnect);
    while (true) {
        cpu.executeNextInstruction();
    }
}
