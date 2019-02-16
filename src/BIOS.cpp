#include "BIOS.hpp"
#include <fstream>

using namespace std;

BIOS::BIOS() : data() {

}

BIOS::~BIOS() {

}

void BIOS::loadBin(const string& path) {
    ifstream file (path, ios::in|ios::binary|ios::ate);
    if (!file.is_open()) {
        exit(1);
    }
    streampos size = file.tellg();
    file.seekg(0, ios::beg);
    file.read(reinterpret_cast<char *>(data), size);
    file.close();
}

uint32_t BIOS::readWord(uint32_t offset) const {
    uint32_t b0 = data[offset + 0];
    uint32_t b1 = data[offset + 1];
    b1 <<= 8;
    uint32_t b2 = data[offset + 2];
    b2 <<= 16;
    uint32_t b3 = data[offset + 3];
    b3 <<= 24;
    return b0 | b1 | b2 | b3;
}


uint8_t BIOS::readByte(uint32_t offset) const {
    return data[offset];
}
