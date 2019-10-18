#pragma once
#include <string>
#include <cstdint>

void readBinary(const std::string& path, uint8_t *data, uint32_t atOrigin);
void readBinary(const std::string& path, uint8_t *data);
uint8_t decimalFromBCDEncodedInt(uint8_t bcdEncoded);
