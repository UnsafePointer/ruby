#pragma once
#include <glad/glad.h>

union Dimensions {
    struct {
        uint32_t width : 16;
        uint32_t height : 16;
    };

    uint32_t _value;
    Dimensions(uint32_t value) : _value(value) {}
    Dimensions(uint32_t width, uint32_t height) : width(width), height(height) {}
};

struct Point {
    GLshort x, y;

    Point();
    Point(GLshort x, GLshort y);
    Point(uint32_t position);
    static Point forTexturePosition(uint16_t position);
    static Point forTexturePage(uint32_t texturePage);
    static Point forClut(uint16_t clutData);
};

struct Color {
    GLubyte r, g, b;

    Color(uint32_t color);
};

enum TextureBlendMode {
    TextureBlendModeNoTexture = 0,
    TextureBlendModeRawTexture,
    TextureBlendModeTextureBlend
};

struct Vertex {
    Point point;
    Color color;
    Point texturePosition;
    GLuint textureBlendMode;
    Point texturePage;
    GLuint textureDepthShift;
    Point clut;
public:
    Vertex(Point point, Color color);
    Vertex(Point point, Color color, Point texturePosition, TextureBlendMode textureBlendMode, Point texturePage, GLuint textureDepthShift, Point clut);
    ~Vertex();
};
