#include "BIOS.hpp"
#include <fstream>
#include <iostream>

using namespace std;

BIOS::BIOS() : data() {

}

BIOS::~BIOS() {

}

void BIOS::loadBin(const string& path) {
    ifstream file (path, ios::in|ios::binary|ios::ate);
    if (!file.is_open()) {
        cout << "Unable to load BIOS" << endl;
        exit(1);
    }
    streampos size = file.tellg();
    file.seekg(0, ios::beg);
    file.read(reinterpret_cast<char *>(data), size);
    file.close();
}
