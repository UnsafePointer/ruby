#pragma once
#include <cstdint>
#include <string>

const uint32_t FrameRateTarget = 60; // Frames per second
const uint32_t SystemClocksPerSecond = 33868800;
const uint32_t SystemClocksPerFrame = 33868800 / 60;
const uint32_t VideoSystemClocksPerScanline = 3413;
// TODO: DotClock depends on the horizontal resolution
const uint32_t VideoSystemClocksPerDot = 6;
const uint32_t ScanlinesPerFrame = 263;
const std::string EmulatorName = "ルビィ";
const uint32_t SecondsPerMinute = 60;
const uint32_t SectorsPerSecond = 75;
