#pragma once
#include <string>
#include <cstdint>

void readBinary(const std::string& path, uint8_t *data, uint32_t atOrigin, int64_t size);
void readBinary(const std::string& path, uint8_t *data);
uint8_t decimalFromBCDEncodedInt(uint8_t bcdEncoded);
