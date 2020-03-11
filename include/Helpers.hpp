#pragma once
#include <string>
#include <cstdint>
#include <filesystem>

void readBinary(const std::filesystem::path& filePath, uint8_t *data, uint32_t atOrigin, int64_t size);
void readBinary(const std::filesystem::path& filePath, uint8_t *data);
uint8_t decimalFromBCDEncodedInt(uint8_t bcdEncoded);
