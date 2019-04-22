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

void TestRunner::setup() {
    if (!runTests) {
        return;
    }
    readHeader();
    string identifier = id();
    if (identifier.compare("PS-X EXE") != 0) {
        printError("Invalid identifier found in file header");
    }
}
