#include "TestRunner.hpp"
#include <string>
#include <fstream>
#include "Output.hpp"
#include <iostream>

using namespace std;

const uint32_t BIOS_A_FUNCTIONS_STEP = 0xB0;
const uint32_t BIOS_STD_OUT_PUT_CHAR = 0x3D;

TestRunner::TestRunner() : cpu(nullptr), runTests(false), header(), ttyBuffer() {}

TestRunner* TestRunner::instance = nullptr;

TestRunner* TestRunner::getInstance() {
    if (instance == nullptr) {
        instance = new TestRunner();
    }
    return instance;
}

void TestRunner::configure(int argc, char* argv[]) {
    if (argc > 1) {
        string argument = string(argv[1]);
        if (argument.compare("--run-tests") == 0) {
            runTests = true;
        }
    }
}

void TestRunner::setCPU(CPU *cpu) {
    this->cpu = cpu;
}

void TestRunner::readHeader() {
    ifstream file ("tests.exe", ios::in|ios::binary|ios::ate);
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
    cpu->transferToRAM("tests.exe", 0x800, fileSize, destinationAddress);

    cpu->setProgramCounter(programCounter());
    cpu->setGlobalPointer(globalPointer());
}

void TestRunner::checkTTY() {
    if (cpu->getProgramCounter() == BIOS_A_FUNCTIONS_STEP) {
        array<uint32_t, 32> registers = cpu->getRegisters();
        uint32_t function = registers[9];
        if (function == BIOS_STD_OUT_PUT_CHAR) {
            ttyBuffer.append(1, registers[4]);
            if (registers[4] == '\n') {
                cout << ttyBuffer;
                ttyBuffer.clear();
            }
        }
    }
}
