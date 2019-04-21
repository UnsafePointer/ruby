#pragma once
#include <glad/glad.h>

struct Point {
    GLshort x, y;
};

struct Color {
    GLubyte r, g, b;
};

struct Vertex {
    Point position;
    Color color;
public:
    Vertex(uint32_t position, uint32_t color);
    ~Vertex();
};
