#include "BIOS.hpp"
#include "Interconnect.hpp"
#include "CPU.hpp"

int main() {
    BIOS bios = BIOS();
    bios.loadBin("SCPH1001.BIN");
    RAM ram = RAM();
    Interconnect interconnect = Interconnect(bios, ram);
    CPU cpu = CPU(interconnect);
    while (true) {
        cpu.executeNextInstruction();
    }
}
