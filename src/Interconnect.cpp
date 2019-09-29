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

Interconnect::Interconnect(unique_ptr<COP0> &cop0,
                           unique_ptr<BIOS> &bios,
                           unique_ptr<RAM> &ram,
                           unique_ptr<GPU> &gpu,
                           unique_ptr<DMA> &dma,
                           unique_ptr<Scratchpad> &scratchpad,
                           unique_ptr<CDROM> &cdrom,
                           unique_ptr<InterruptController> &interruptController,
                           unique_ptr<Expansion1> &expansion1,
                           unique_ptr<Timer0> &timer0,
                           unique_ptr<Timer1> &timer1,
                           unique_ptr<Timer2> &timer2,
                           unique_ptr<Controller> &controller) : cop0(cop0),
                                                                 bios(bios),
                                                                 ram(ram),
                                                                 gpu(gpu),
                                                                 dma(dma),
                                                                 scratchpad(scratchpad),
                                                                 cdrom(cdrom),
                                                                 interruptController(interruptController),
                                                                 expansion1(expansion1),
                                                                 timer0(timer0),
                                                                 timer1(timer1),
                                                                 timer2(timer2),
                                                                 controller(controller),
                                                                 ranges({{
                                                                    Range(RangeIdentifier::RAMR, 0x00000000, 2*1024*1024),
                                                                    Range(RangeIdentifier::Expansion1R, 0x1f000000, 512*1024),
                                                                    Range(RangeIdentifier::ScratchpadR, 0x1f800000, 1024),
                                                                    Range(RangeIdentifier::MemoryControlR, 0x1f801000, 36),
                                                                    Range(RangeIdentifier::ControllerRegisterR, 0x1f801040, 16),
                                                                    Range(RangeIdentifier::RamSizeR, 0x1f801060, 4),
                                                                    Range(RangeIdentifier::InterruptRequestControlR, 0x1f801070, 8),
                                                                    Range(RangeIdentifier::DMARegisterR, 0x1f801080, 0x80),
                                                                    Range(RangeIdentifier::Timer0RegisterR, 0x1f801100, 16),
                                                                    Range(RangeIdentifier::Timer1RegisterR, 0x1f801110, 16),
                                                                    Range(RangeIdentifier::Timer2RegisterR, 0x1f801120, 16),
                                                                    Range(RangeIdentifier::CDROMRegisterR, 0x1f801800, 4),
                                                                    Range(RangeIdentifier::GPURegisterR, 0x1f801810, 8),
                                                                    Range(RangeIdentifier::SoundProcessingUnitR, 0x1f801c00, 640),
                                                                    Range(RangeIdentifier::Expansion2R, 0x1f802000, 66),
                                                                    Range(RangeIdentifier::BIOSR, 0x1fc00000, 512 * 1024),
                                                                    Range(RangeIdentifier::CacheControlR, 0xfffe0130, 4),
                                                                        }})
{
    bios->loadBin("SCPH1001.BIN");
    EmulatorRunner *emulatorRunner = EmulatorRunner::getInstance();
    if (emulatorRunner->shouldRunTests()) {
        expansion1->loadBin("expansion/EXPNSION.BIN");
    }
}

Interconnect::~Interconnect() {}

uint32_t Interconnect::maskRegion(uint32_t address) const {
    uint8_t index = address >> 29;
    return address & regionMask[index];
}

std::optional<Range> Interconnect::findRangeForAddress(uint32_t address, std::array<Range, 17> const *ranges, uint32_t bottom, uint32_t top) const {
    uint32_t size = top - bottom;
    if (size < 1) {
        return nullopt;
    }
    uint32_t middle = size / 2;
    Range range = ranges->at(bottom + middle);
    int32_t result = range.contains(address);
    if (result == 0) {
        return { range };
    }
    if (size == 1) {
        return nullopt;
    }
    if (result < 0) {
        return findRangeForAddress(address, ranges, bottom, bottom + middle);
    } else {
        return findRangeForAddress(address, ranges, bottom + middle, top);
    }
}

void Interconnect::transferToRAM(string path, uint32_t origin, uint32_t size, uint32_t destination) {
    uint32_t maskedDestination = maskRegion(destination);
    ram->receiveTransfer(path, origin, size, maskedDestination);
}

void Interconnect::dumpRAM() {
    ram->dump();
}
