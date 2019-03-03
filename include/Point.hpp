#pragma once
#include <glad/glad.h>

class Point {
    GLshort x;
    GLshort y;
public:
    Point(uint32_t value);
    ~Point();
};
