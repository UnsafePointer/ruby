#include "Interconnect.hpp"
#include "Range.hpp"
#include <iostream>

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
const Range ramRange = Range(0x00000000, RAM_SIZE);
const Range biosRange = Range(0x1fc00000, 512 * 1024);
const Range memoryControlRange = Range(0x1f801000, 36);
const Range ramSizeRange = Range(0x1f801060, 4);
const Range cacheControlRange = Range(0xfffe0130, 4);
const Range soundProcessingUnitRange = Range(0x1f801c00, 640);
const Range expansion2Range = Range(0x1f802000, 66);

Interconnect::Interconnect(BIOS &bios, RAM &ram) : bios(bios), ram(ram) {
}

Interconnect::~Interconnect() {
}

uint32_t Interconnect::maskRegion(uint32_t address) const {
    uint8_t index = address >> 29;
    return address & regionMask[index];
}

uint32_t Interconnect::readWord(uint32_t address) const {
    if (address % 4 != 0) {
        exit(1);
    }
    uint32_t absoluteAddress = maskRegion(address);

    optional<uint32_t> offset = biosRange.contains(absoluteAddress);
    if (offset) {
        return bios.readWord(*offset);
    }
    offset = ramRange.contains(absoluteAddress);
    if (offset) {
        return ram.readWord(*offset);
    }
    cout << "Unhandled read at: 0x" << hex << address << endl;
    exit(1);
}

uint8_t Interconnect::readByte(uint32_t address) const {
    uint32_t absoluteAddress = maskRegion(address);

    optional<uint32_t> offset = biosRange.contains(absoluteAddress);
    if (offset) {
        return bios.readByte(*offset);
    }

    cout << "Unhandled read byte at: 0x" << hex << address << endl;
    exit(1);
}

void Interconnect::storeWord(uint32_t address, uint32_t value) const {
    if (address % 4 != 0) {
        exit(1);
    }
    uint32_t absoluteAddress = maskRegion(address);
    optional<uint32_t> offset;
    offset = memoryControlRange.contains(absoluteAddress);
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
                cout << "Unhandled Memory Control write at offset: 0x" << hex << *offset << endl;
                break;
            }
        }
        return;
    }
    offset = ramSizeRange.contains(absoluteAddress);
    if (offset) {
        cout << "Unhandled RAM Control write at offset: 0x" << hex << *offset << endl;
        return;
    }
    offset = cacheControlRange.contains(absoluteAddress);
    if (offset) {
        cout << "Unhandled Cache Control write offset: 0x" << hex << *offset << endl;
        return;
    }
    offset = ramRange.contains(absoluteAddress);
    if (offset) {
        ram.storeWord(*offset, value);
        return;
    }
    cout << "Unhandled write at: 0x" << hex << address << endl;
    exit(1);
}

void Interconnect::storeHalfWord(uint32_t address, uint16_t value) const {
    if (address % 2 != 0) {
        exit(1);
    }
    uint32_t absoluteAddress = maskRegion(address);
    optional<uint32_t> offset;
    offset = soundProcessingUnitRange.contains(absoluteAddress);
    if (offset) {
        cout << "Unhandled Sound Processing Unit write at offset: 0x" << hex << *offset << endl;
        return;
    }
    cout << "Unhandled half write at: 0x" << hex << address << endl;
    exit(1);
}

void Interconnect::storeByte(uint32_t address, uint8_t value) const {
    uint32_t absoluteAddress = maskRegion(address);
    optional<uint32_t> offset;
    offset = expansion2Range.contains(absoluteAddress);
    if (offset) {
        cout << "Unhandled Expansion 2 write at offset: 0x" << hex << *offset << endl;
        return;
    }
    cout << "Unhandled byte write at: 0x" << hex << address << endl;
    exit(1);
}
