#include "BIOS.hpp"
#include "Output.hpp"
#include "Helpers.hpp"

using namespace std;

BIOS::BIOS() : data() {

}

BIOS::~BIOS() {

}

void BIOS::loadBin(const string& path) {
    readBinary(path, data);
}
