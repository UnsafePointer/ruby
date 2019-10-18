#include "Helpers.hpp"
#include <fstream>
#include "Output.hpp"
#include <iostream>

using namespace std;

void readBinary(const std::string& path, uint8_t *data, uint32_t atOrigin) {
    ifstream file = ifstream(path, ios::in | ios::binary | ios::ate);
    if (!file.is_open()) {
        cout << format("Unable to load binary at path %s", path.c_str()) << endl;
        exit(1);
    }
    streampos size = file.tellg();
    file.seekg(atOrigin, ios::beg);
    file.read(reinterpret_cast<char *>(data), size);
    file.close();
}

void readBinary(const string& path, uint8_t *data) {
    readBinary(path, data, 0);
}

uint8_t decimalFromBCDEncodedInt(uint8_t bcdEncoded) {
    uint8_t high = (bcdEncoded >> 4) & 0xF;
    uint8_t low = bcdEncoded & 0xF;

    uint8_t value = high * 10 + low;
    return value;
}
