#pragma once
#include <string>
#include <cstdint>
#include <filesystem>
#include <tuple>

void readBinary(const std::filesystem::path& filePath, uint8_t *data, uint32_t atOrigin, int64_t size);
void readBinary(const std::filesystem::path& filePath, uint8_t *data);
uint8_t decimalFromBCDEncodedInt(uint8_t bcdEncoded);
std::tuple<uint8_t, uint8_t, uint8_t> minutesSecondsSectorsFromLogicalABlockddressing(unsigned int lba);
uint8_t BCDEncodedIntFromDecimal(unsigned int decimal);
uint32_t countLeadingZeroes(uint32_t value);
int16_t saturate(int32_t value);
