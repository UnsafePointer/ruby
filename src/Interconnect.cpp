#include "Interconnect.hpp"
#include "Range.hpp"
#include "EmulatorRunner.hpp"

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

Interconnect::Interconnect(LogLevel logLevel, std::unique_ptr<COP0> &cop0, unique_ptr<BIOS> &bios, unique_ptr<RAM> &ram, unique_ptr<GPU> &gpu, unique_ptr<DMA> &dma, unique_ptr<Scratchpad> &scratchpad, unique_ptr<CDROM> &cdrom, unique_ptr<InterruptController> &interruptController, unique_ptr<Expansion1> &expansion1, std::unique_ptr<Timer0> &timer0, std::unique_ptr<Timer1> &timer1, std::unique_ptr<Timer2> &timer2, std::unique_ptr<Controller> &controller, std::unique_ptr<SPU> &spu) : logger(logLevel), cop0(cop0), bios(bios), ram(ram), gpu(gpu), dma(dma), scratchpad(scratchpad), cdrom(cdrom), interruptController(interruptController), expansion1(expansion1), timer0(timer0), timer1(timer1), timer2(timer2), controller(controller), spu(spu) {
    filesystem::path biosFilePath = filesystem::current_path() / "SCPH1001.BIN";
    bios->loadBin(biosFilePath);
    EmulatorRunner *emulatorRunner = EmulatorRunner::getInstance();
    if (emulatorRunner->shouldRunTests()) {
        filesystem::path expansionFilePath = filesystem::current_path() / "expansion" / "EXPNSION.BIN";
        expansion1->loadBin(expansionFilePath);
    } else if (emulatorRunner->shouldLoadExpansionROM()) {
        expansion1->loadBin(emulatorRunner->romFilePath());
    }
}

Interconnect::~Interconnect() {}

uint32_t Interconnect::maskRegion(uint32_t address) const {
    uint8_t index = address >> 29;
    return address & regionMask[index];
}

void Interconnect::transferToRAM(filesystem::path filePath, uint32_t origin, uint32_t size, uint32_t destination) {
    uint32_t maskedDestination = maskRegion(destination);
    ram->receiveTransfer(filePath, origin, size, maskedDestination);
}

void Interconnect::dumpRAM() {
    ram->dump();
}
