#include "RAM.hpp"
#include <algorithm>

using namespace std;

RAM::RAM() : data() {
    fill_n(data, RAM_SIZE, 0xCA);
}

RAM::~RAM() {

}
