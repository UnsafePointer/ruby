#include "EmulatorRunner.hpp"
#include <fstream>
#include <algorithm>
#include "CPU.hpp"

using namespace std;

EmulatorRunner::EmulatorRunner() : logger(LogLevel::NoLog), emulator(nullptr), runTests(false), loadExpansionROM(false), exeFile(), binFile(), romFile(), header() {}

EmulatorRunner* EmulatorRunner::instance = nullptr;

EmulatorRunner* EmulatorRunner::getInstance() {
    if (instance == nullptr) {
        instance = new EmulatorRunner();
    }
    return instance;
}

bool EmulatorRunner::checkOption(char** begin, char** end, const std::string &option) {
    return find(begin, end, option) != end;
}

char* EmulatorRunner::getOptionValue(char** begin, char** end, const std::string &option) {
    char **iterator = find(begin, end, option);
    if (iterator != end && iterator++ != end) {
        return *iterator;
    }
    return NULL;
}

void EmulatorRunner::configure(int argc, char* argv[]) {
    if (argc <= 1) {
        return;
    }
    bool argumentFound = false;
    if (checkOption(argv, argv + argc, "--exe")) {
        char *path = getOptionValue(argv, argv + argc, "--exe");
        if (path == NULL) {
            logger.logError("Incorrect argument passed. See README.md for usage.");
        }
        runTests = true;
        exeFile = filesystem::current_path() / string(path);
        if (!filesystem::exists(exeFile)) {
            logger.logError("The provided --exe filepath doesn't exist.");
        }
        argumentFound = true;
    } else if (checkOption(argv, argv + argc, "--bin")) {
        char *path = getOptionValue(argv, argv + argc, "--bin");
        if (path == NULL) {
            logger.logError("Incorrect argument passed. See README.md for usage.");
        }
        binFile = filesystem::current_path() / string(path);
        if (!filesystem::exists(binFile)) {
            logger.logError("The provided --bin filepath doesn't exist.");
        }
        argumentFound = true;
    }
    if (checkOption(argv, argv + argc, "--rom")) {
        char *path = getOptionValue(argv, argv + argc, "--rom");
        if (path == NULL) {
            logger.logError("Incorrect argument passed. See README.md for usage.");
        }
        loadExpansionROM = true;
        romFile = filesystem::current_path() / string(path);
        if (!filesystem::exists(romFile)) {
            logger.logError("The provided --rom filepath doesn't exist.");
        }
        argumentFound = true;
    }
    if (!argumentFound) {
        logger.logError("Incorrect argument passed. See README.md for usage.");
    }
}

void EmulatorRunner::setEmulator(Emulator *emulator) {
    this->emulator = emulator;
    this->emulator->loadCDROMImageFile(binFile);
}

void EmulatorRunner::readHeader() {
    ifstream file = ifstream(exeFile, ios::in|ios::binary|ios::ate);
    if (!file.is_open()) {
        logger.logError("Unable to tests.exe binary");
    }
    file.seekg(0, ios::beg);
    file.read(reinterpret_cast<char *>(header), TEST_HEADER_SIZE);
    file.close();
}

string EmulatorRunner::id() {
    return string(reinterpret_cast<char *>(header), 8);
}

uint32_t EmulatorRunner::loadWord(uint32_t offset) {
    uint32_t value = 0;
    for (uint8_t i = 0; i < 4; i++) {
        value |= (((uint32_t)header[offset + i]) << (i * 8));
    }
    return value;
}

bool EmulatorRunner::shouldRunTests() {
    return runTests;
}

bool EmulatorRunner::shouldLoadExpansionROM() {
    return loadExpansionROM;
}

std::filesystem::path EmulatorRunner::romFilePath() {
    return romFile;
}

uint32_t EmulatorRunner::programCounter() {
    return loadWord(0x10);
}

uint32_t EmulatorRunner::globalPointer() {
    return loadWord(0x14);
}

uint32_t EmulatorRunner::initialStackFramePointerBase() {
    return loadWord(0x30);
}

uint32_t EmulatorRunner::initialStackFramePointeroffset() {
    return loadWord(0x34);
}

uint32_t EmulatorRunner::destinationAddress() {
    return loadWord(0x18);
}

uint32_t EmulatorRunner::fileSize() {
    return loadWord(0x1C);
}

void EmulatorRunner::setup() {
    if (!shouldRunTests()) {
        return;
    }
    readHeader();
    string identifier = id();
    if (identifier.compare("PS-X EXE") != 0) {
        logger.logError("Invalid identifier found in file header");
    }
    uint32_t destinationAddress = this->destinationAddress();
    uint32_t fileSize = this->fileSize();
    if (fileSize % 0x800 != 0) {
        logger.logError("Invalid file size found in file header");
    }
    emulator->transferToRAM(exeFile, 0x800, fileSize, destinationAddress);

    CPU *cpu = emulator->getCPU();
    cpu->setProgramCounter(programCounter());
    cpu->setGlobalPointer(globalPointer());
    cpu->setStackPointer(initialStackFramePointerBase() + initialStackFramePointeroffset());
    cpu->setFramePointer(initialStackFramePointerBase() + initialStackFramePointeroffset());
}
