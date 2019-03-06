#pragma once
#include <glad/glad.h>

class Color {
    GLubyte red;
    GLubyte green;
    GLubyte blue;
public:
    Color(uint32_t value);
    Color(GLubyte red, GLubyte green, GLubyte blue);
    ~Color();
};
