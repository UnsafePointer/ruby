#include "Scratchpad.hpp"
#include <algorithm>

using namespace std;

Scratchpad::Scratchpad() {
    fill_n(data, SCRATCHPAD_SIZE, 0xCA);
}

Scratchpad::~Scratchpad() {

}
