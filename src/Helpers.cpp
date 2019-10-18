#include "Helpers.hpp"
#include <fstream>
#include "Output.hpp"
#include <iostream>

using namespace std;

void readBinary(const std::string& path, uint8_t *data, uint32_t atOrigin, int64_t size) {
    ifstream file = ifstream(path, ios::in | ios::binary | ios::ate);
    if (!file.is_open()) {
        cout << format("Unable to load binary at path %s", path.c_str()) << endl;
        exit(1);
    }
    unsigned long long sizeToRead;
    if (size == -1) {
        sizeToRead = file.tellg();
    } else {
        sizeToRead = size;
    }
    file.seekg(atOrigin, ios::beg);
    file.read(reinterpret_cast<char *>(data), sizeToRead);
    file.close();
}

void readBinary(const string& path, uint8_t *data) {
    readBinary(path, data, 0, -1);
}

uint8_t decimalFromBCDEncodedInt(uint8_t bcdEncoded) {
    uint8_t high = (bcdEncoded >> 4) & 0xF;
    uint8_t low = bcdEncoded & 0xF;

    uint8_t value = high * 10 + low;
    return value;
}
