#include "RAM.hpp"
#include <algorithm>
#include <fstream>
#include "Helpers.hpp"

using namespace std;

RAM::RAM() : data() {
}

RAM::~RAM() {

}

void RAM::receiveTransfer(filesystem::path filePath, uint32_t origin, uint32_t size, uint32_t destination) {
    uint8_t *dataDestination = &data[destination];
    readBinary(filePath, dataDestination, origin, size);
}

void RAM::dump() {
    filesystem::path ramBinFilePath = filesystem::current_path() / "ram.bin";
    std::ofstream(ramBinFilePath, std::ios::binary).write(reinterpret_cast<char *>(data), RAM_SIZE);
}
