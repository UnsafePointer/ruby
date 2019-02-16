#include "RegisterIndex.hpp"

RegisterIndex::RegisterIndex() : index(0) {

}

RegisterIndex::RegisterIndex(uint8_t index) : index(index) {

}

RegisterIndex::~RegisterIndex() {

}

uint8_t RegisterIndex::idx() {
    return index;
}
