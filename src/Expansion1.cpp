#include "Expansion1.hpp"
#include "Helpers.hpp"

using namespace std;

Expansion1::Expansion1() : data() {

}

Expansion1::~Expansion1() {

}

void Expansion1::loadBin(const std::filesystem::path& filePath) {
    readBinary(filePath, data);
}
