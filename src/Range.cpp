#include "Range.hpp"

using namespace std;

Range::Range(uint32_t start, uint32_t length) : start(start), length(length) {

}

Range::~Range() {

}

std::optional<uint32_t> Range::contains(uint32_t address) const {
    if (address >= start && address <= (start + length)) {
        uint32_t offset = address - start;
        return { offset };
    } else {
        return nullopt;
    }
 }
