#include "Helpers.hpp"
#include <fstream>
#include "Output.hpp"

using namespace std;

void readBinary(const string& path, uint8_t *data) {
    ifstream file = ifstream(path, ios::in|ios::binary|ios::ate);
    if (!file.is_open()) {
        printError("Unable to load BIOS");
    }
    streampos size = file.tellg();
    file.seekg(0, ios::beg);
    file.read(reinterpret_cast<char *>(data), size);
    file.close();
}

uint8_t decimalFromBCDEncodedInt(uint8_t bcdEncoded) {
    uint8_t high = (bcdEncoded >> 4) & 0xF;
    uint8_t low = bcdEncoded & 0xF;

    uint8_t value = high * 10 + low;
    return value;
}
