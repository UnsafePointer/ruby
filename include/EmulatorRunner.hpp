#pragma once
#include <memory>
#include <string>
#include <filesystem>
#include "Emulator.hpp"
#include "Logger.hpp"

// XXXX_NNN.NN (Boot-Executable) (filename specified in SYSTEM.CNF)
// FILENAME.EXE (General-Purpose Executable)
// PSX executables are having an 800h-byte header, followed by the code/data.
//   000h-007h ASCII ID "PS-X EXE"
//   008h-00Fh Zerofilled
//   010h      Initial PC                   (usually 80010000h, or higher)
//   014h      Initial GP/R28               (usually 0)
//   018h      Destination Address in RAM   (usually 80010000h, or higher)
//   01Ch      Filesize (must be N*800h)    (excluding 800h-byte header)
//   020h      Unknown/Unused               (usually 0)
//   024h      Unknown/Unused               (usually 0)
//   028h      Memfill Start Address        (usually 0) (when below Size=None)
//   02Ch      Memfill Size in bytes        (usually 0) (0=None)
//   030h      Initial SP/R29 & FP/R30 Base (usually 801FFFF0h) (or 0=None)
//   034h      Initial SP/R29 & FP/R30 Offs (usually 0, added to above Base)
//   038h-04Bh Reserved for A(43h) Function (should be zerofilled in exefile)
//   04Ch-xxxh ASCII marker
//              "Sony Computer Entertainment Inc. for Japan area"
//              "Sony Computer Entertainment Inc. for Europe area"
//              "Sony Computer Entertainment Inc. for North America area"
//              (or often zerofilled in some homebrew files)
//              (the BIOS doesn't verify this string, and boots fine without it)
//   xxxh-7FFh Zerofilled
//   800h...   Code/Data                  (loaded to entry[018h] and up)

const uint32_t TEST_HEADER_SIZE = 2048;

class EmulatorRunner {
    static EmulatorRunner* instance;

    Logger logger;
    Emulator *emulator;
    bool runTests;
    bool loadExpansionROM;
    std::filesystem::path exeFile;
    std::filesystem::path binFile;
    std::filesystem::path romFile;
    uint8_t header[TEST_HEADER_SIZE];

    void readHeader();
    std::string id();
    uint32_t loadWord(uint32_t offset);
    uint32_t destinationAddress();
    uint32_t fileSize();

    bool checkOption(char** begin, char** end, const std::string &option);
    char* getOptionValue(char** begin, char** end, const std::string &option);

    EmulatorRunner();
public:
    static EmulatorRunner* getInstance();

    void configure(int argc, char* argv[]);
    void setEmulator(Emulator *emulator);
    bool shouldRunTests();
    bool shouldLoadExpansionROM();
    std::filesystem::path romFilePath();
    uint32_t programCounter();
    uint32_t globalPointer();
    uint32_t initialStackFramePointerBase();
    uint32_t initialStackFramePointeroffset();
    void setup();
};
