#include "Interconnect.hpp"
#include "Range.hpp"
#include <iostream>

using namespace std;

const Range biosRange = Range(0xbfc00000, 512 * 1024);
const Range memoryControlRange = Range(0x1f801000, 36);
const Range ramSizeRange = Range(0x1f801060, 4);
const Range cacheControlRange = Range(0xfffe0130, 4);
const Range ramRange = Range(0xa0000000, RAM_SIZE);

Interconnect::Interconnect(BIOS &bios, RAM &ram) : bios(bios), ram(ram) {
}

Interconnect::~Interconnect() {
}

uint32_t Interconnect::readWord(uint32_t address) const {
    if (address % 4 != 0) {
        exit(1);
    }

    optional<uint32_t> offset = biosRange.contains(address);
    if (offset) {
        return bios.readWord(*offset);
    }
    offset = ramRange.contains(address);
    if (offset) {
        return ram.readWord(*offset);
    }
    exit(1);
}

void Interconnect::storeWord(uint32_t address, uint32_t value) const {
    if (address % 4 != 0) {
        exit(1);
    }
    optional<uint32_t> offset;
    offset = memoryControlRange.contains(address);
    if (offset) {
        // PlayStation BIOS should not set these to any different value
        switch (*offset) {
            case 0: {
                if (value != 0x1f000000) {
                    exit(1);
                }
                break;
            }
            case 4: {
                if (value != 0x1f802000) {
                    exit(1);
                }
                break;
            }
            default: {
                cout << "Unhandled Memory Control write at: 0x" << hex << address << endl;
                break;
            }
        }
        return;
    }
    offset = ramSizeRange.contains(address);
    if (offset) {
        cout << "Unhandled RAM Control write at: 0x" << hex << address << endl;
        return;
    }
    offset = cacheControlRange.contains(address);
    if (offset) {
        cout << "Unhandled Cache Control write at: 0x" << hex << address << endl;
        return;
    }
    offset = ramRange.contains(address);
    if (offset) {
        ram.storeWord(*offset, value);
        return;
    }
    cout << "Unhandled write at: 0x" << hex << address << endl;
    exit(1);
}
