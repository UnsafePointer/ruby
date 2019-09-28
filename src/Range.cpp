#include "Range.hpp"

using namespace std;

Range::Range(RangeIdentifier identifier, uint32_t start, uint32_t length) : identifier(identifier), start(start), length(length) {

}

Range::~Range() {

}

int32_t Range::contains(uint32_t address) const {
    if (address >= start && address < (start + length)) {
        return 0;
    } else if (address < start) {
        return -1;
    } else {
        return 1;
    }
 }

 uint32_t Range::offset(uint32_t address) const {
     return address - start;
 }

