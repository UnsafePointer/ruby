#include "Helpers.hpp"
#include <fstream>
#include "Output.hpp"
#include <iostream>
#include "Constants.h"

using namespace std;

void readBinary(const filesystem::path& filePath, uint8_t *data, uint32_t atOrigin, int64_t size) {
    ifstream file = ifstream(filePath, ios::in | ios::binary | ios::ate);
    if (!file.is_open()) {
        cout << format("Unable to load binary at path %s", filePath.string().c_str()) << endl;
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

void readBinary(const filesystem::path& filePath, uint8_t *data) {
    readBinary(filePath, data, 0, -1);
}

uint8_t decimalFromBCDEncodedInt(uint8_t bcdEncoded) {
    uint8_t high = (bcdEncoded >> 4) & 0xF;
    uint8_t low = bcdEncoded & 0xF;

    uint8_t value = high * 10 + low;
    return value;
}

tuple<uint8_t, uint8_t, uint8_t> minutesSecondsSectorsFromLogicalABlockddressing(unsigned int lba) {
    uint8_t sectors = lba % SectorsPerSecond;
    lba /= SectorsPerSecond;

    uint8_t seconds = lba % SecondsPerMinute;
    lba /= SecondsPerMinute;

    uint8_t minutes = lba;

    return { minutes, seconds, sectors };
}

uint8_t BCDEncodedIntFromDecimal(unsigned int decimal) {
    uint8_t high = decimal / 10;
    uint8_t low = decimal % 10;
    return high | low;
}

uint32_t countLeadingZeroes(uint32_t value) {
    uint32_t sign = value >> 31;
    uint32_t leadingZeroes = 0;
    for (unsigned int i = 0; i < 32; i++) {
        if ((value >> 31) != sign) {
            break;
        }
        leadingZeroes++;
        value <<= 1;
    }
    return leadingZeroes;
}
