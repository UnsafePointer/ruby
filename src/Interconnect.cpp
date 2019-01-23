#include "Interconnect.hpp"
#include "Range.hpp"

using namespace std;

const Range biosRange = Range(0xbfc00000, 512 * 1024);

Interconnect::Interconnect(BIOS &bios) : bios(bios) {
}

Interconnect::~Interconnect() {
}

uint32_t Interconnect::readWord(uint32_t address) const {
    optional<uint32_t> offset = biosRange.contains(address);
    if (offset) {
        return bios.readWord(*offset);
    }

    exit(1);
}
