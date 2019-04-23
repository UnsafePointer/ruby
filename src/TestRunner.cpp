#include "TestRunner.hpp"
#include <string>
#include <fstream>
#include "Output.hpp"

using namespace std;

TestRunner::TestRunner(int argc, char* argv[], std::unique_ptr<CPU> &cpu) : cpu(cpu), header() {
    runTests = false;
    if (argc > 1) {
        string argument = string(argv[1]);
        if (argument.compare("--run-tests") == 0) {
            runTests = true;
        }
    }
}

TestRunner::~TestRunner() {

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

uint32_t TestRunner::programCounter() {
    return loadWord(0x10);
}

uint32_t TestRunner::destinationAddress() {
    return loadWord(0x18);
}

uint32_t TestRunner::fileSize() {
    return loadWord(0x1C);
}

void TestRunner::setup() {
    if (!runTests) {
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
    cpu->setProgramCounter(programCounter);
}
