#include "RAM.hpp"
#include <algorithm>
#include <fstream>
#include "Helpers.hpp"

using namespace std;

RAM::RAM() : data() {
}

RAM::~RAM() {

}

void RAM::receiveTransfer(std::string path, uint32_t origin, uint32_t size, uint32_t destination) {
    uint8_t *dataDestination = &data[destination];
    readBinary(path, dataDestination, origin, size);
}

void RAM::dump() {
    std::ofstream("ram.bin", std::ios::binary).write(reinterpret_cast<char *>(data), RAM_SIZE);
}
