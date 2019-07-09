#include "Interconnect.hpp"
#include "Range.hpp"
#include "TestRunner.hpp"

using namespace std;

const uint32_t regionMask[8] = {
    // KUSEG: 2048MB
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    // KSEG0: 512MB
    0x7fffffff,
    // KSEG1: 512MB
    0x1fffffff,
    // KSEG2: 1024MB
    0xffffffff, 0xffffffff,
};

Interconnect::Interconnect(unique_ptr<BIOS> &bios, unique_ptr<RAM> &ram, unique_ptr<GPU> &gpu, unique_ptr<DMA> &dma, unique_ptr<Scratchpad> &scratchpad, unique_ptr<CDROM> &cdrom, unique_ptr<InterruptController> &interruptController, unique_ptr<Expansion1> &expansion1, std::unique_ptr<Timer0> &timer0, std::unique_ptr<Timer1> &timer1, std::unique_ptr<Timer2> &timer2) : bios(bios), ram(ram), gpu(gpu), dma(dma), scratchpad(scratchpad), cdrom(cdrom), interruptController(interruptController), expansion1(expansion1), timer0(timer0), timer1(timer1), timer2(timer2) {
    bios->loadBin("SCPH1001.BIN");
    TestRunner *testRunner = TestRunner::getInstance();
    if (testRunner->shouldRunTests()) {
        expansion1->loadBin("expansion/EXPNSION.BIN");
    }
}

Interconnect::~Interconnect() {}

uint32_t Interconnect::maskRegion(uint32_t address) const {
    uint8_t index = address >> 29;
    return address & regionMask[index];
}

void Interconnect::transferToRAM(string path, uint32_t origin, uint32_t size, uint32_t destination) {
    uint32_t maskedDestination = maskRegion(destination);
    ram->receiveTransfer(path, origin, size, maskedDestination);
}

void Interconnect::dumpRAM() {
    ram->dump();
}
