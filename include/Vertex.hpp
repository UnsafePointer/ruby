#pragma once
#include <glad/glad.h>

struct Point {
    GLshort x, y;
};

struct Color {
    GLubyte r, g, b;
};

enum TextureBlendMode {
    TextureBlendModeNoTexture = 0,
    TextureBlendModeRawTexture,
    TextureBlendModeTextureBlend
};

struct Vertex {
    Point position;
    Color color;
    Point texturePosition;
    GLuint textureBlendMode;
    Point texturePage;
public:
    Vertex(uint32_t position, uint32_t color);
    Vertex(uint32_t position, uint32_t color, uint16_t texturePosition, TextureBlendMode textureBlendMode, uint32_t texturePage);
    ~Vertex();
};
