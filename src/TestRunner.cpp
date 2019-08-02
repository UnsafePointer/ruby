#include "TestRunner.hpp"
#include <fstream>
#include <algorithm>
#include "Output.hpp"
#include "CPU.hpp"

using namespace std;

TestRunner::TestRunner() : emulator(nullptr), runTests(false), exeFile(), header() {}

TestRunner* TestRunner::instance = nullptr;

TestRunner* TestRunner::getInstance() {
    if (instance == nullptr) {
        instance = new TestRunner();
    }
    return instance;
}

bool TestRunner::checkOption(char** begin, char** end, const std::string &option) {
    return find(begin, end, option) != end;
}

char* TestRunner::getOptionValue(char** begin, char** end, const std::string &option) {
    char **iterator = find(begin, end, option);
    if (iterator != end && iterator++ != end) {
        return *iterator;
    }
    return NULL;
}

void TestRunner::configure(int argc, char* argv[]) {
    if (argc <= 1) {
        return;
    }
    bool argumentFound = false;
    if (checkOption(argv, argv + argc, "--exe")) {
        char *path = getOptionValue(argv, argv + argc, "--exe");
        if (path == NULL) {
            printError("Incorrect argument passed. See README.md for usage.");
        }
        runTests = true;
        exeFile = string(path);
        argumentFound = true;
    }
    if (!argumentFound) {
        printError("Incorrect argument passed. See README.md for usage.");
    }
}

void TestRunner::setEmulator(Emulator *emulator) {
    this->emulator = emulator;
}

void TestRunner::readHeader() {
    ifstream file = ifstream(exeFile, ios::in|ios::binary|ios::ate);
    if (!file.is_open()) {
        printError("Unable to tests.exe binary");
    }
    file.seekg(0, ios::beg);
    file.read(reinterpret_cast<char *>(header), TEST_HEADER_SIZE);
    file.close();
}

string TestRunner::id() {
    return string(reinterpret_cast<char *>(header), 8);
}

uint32_t TestRunner::loadWord(uint32_t offset) {
    uint32_t value = 0;
    for (uint8_t i = 0; i < 4; i++) {
        value |= (((uint32_t)header[offset + i]) << (i * 8));
    }
    return value;
}

bool TestRunner::shouldRunTests() {
    return runTests;
}

uint32_t TestRunner::programCounter() {
    return loadWord(0x10);
}

uint32_t TestRunner::globalPointer() {
    return loadWord(0x14);
}

uint32_t TestRunner::initialStackFramePointerBase() {
    return loadWord(0x30);
}

uint32_t TestRunner::initialStackFramePointeroffset() {
    return loadWord(0x34);
}

uint32_t TestRunner::destinationAddress() {
    return loadWord(0x18);
}

uint32_t TestRunner::fileSize() {
    return loadWord(0x1C);
}

void TestRunner::setup() {
    if (!shouldRunTests()) {
        return;
    }
    readHeader();
    string identifier = id();
    if (identifier.compare("PS-X EXE") != 0) {
        printError("Invalid identifier found in file header");
    }
    uint32_t destinationAddress = this->destinationAddress();
    uint32_t fileSize = this->fileSize();
    if (fileSize % 0x800 != 0) {
        printError("Invalid file size found in file header");
    }
    emulator->transferToRAM(exeFile, 0x800, fileSize, destinationAddress);

    CPU *cpu = emulator->getCPU();
    cpu->setProgramCounter(programCounter());
    cpu->setGlobalPointer(globalPointer());
    cpu->setStackPointer(initialStackFramePointerBase() + initialStackFramePointeroffset());
    cpu->setFramePointer(initialStackFramePointerBase() + initialStackFramePointeroffset());
}
