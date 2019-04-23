#include "RAM.hpp"
#include <algorithm>
#include <fstream>
#include "Output.hpp"

using namespace std;

RAM::RAM() : data() {
    fill_n(data, RAM_SIZE, 0xCA);
}

RAM::~RAM() {

}

void RAM::receiveTransfer(std::string path, uint32_t origin, uint32_t size, uint32_t destination) {
    ifstream file (path, ios::in|ios::binary|ios::ate);
    if (!file.is_open()) {
        printError("Unable to load binary");
    }
    file.seekg(origin);
    uint8_t *dataDestination = &data[destination];
    file.read(reinterpret_cast<char *>(dataDestination), size);
    file.close();
}
