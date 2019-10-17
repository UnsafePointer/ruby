#include "RAM.hpp"
#include <algorithm>
#include <fstream>
#include "Output.hpp"
#include <iostream>

using namespace std;

RAM::RAM() : data() {
}

RAM::~RAM() {

}

void RAM::receiveTransfer(std::string path, uint32_t origin, uint32_t size, uint32_t destination) {
    ifstream file (path, ios::in|ios::binary|ios::ate);
    if (!file.is_open()) {
        cout << "Unable to load binary" << endl;
    }
    file.seekg(origin);
    uint8_t *dataDestination = &data[destination];
    file.read(reinterpret_cast<char *>(dataDestination), size);
    file.close();
}

void RAM::dump() {
    std::ofstream("ram.bin", std::ios::binary).write(reinterpret_cast<char *>(data), RAM_SIZE);
}
