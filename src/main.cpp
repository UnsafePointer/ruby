#include "BIOS.hpp"
#include "Interconnect.hpp"
#include "CPU.hpp"

int main() {
    BIOS bios = BIOS();
    bios.loadBin("SCPH1001.BIN");
    Interconnect interconnect = Interconnect(bios);
    CPU cpu = CPU(interconnect);
    while (true) {
        cpu.executeNext();
    }
}
