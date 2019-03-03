#include "Point.hpp"

Point::Point(uint32_t value) {
    x = ((GLshort)(value & 0xffff));
    y = ((GLshort)((value >> 16) & 0xffff));
}

Point::~Point() {

}
