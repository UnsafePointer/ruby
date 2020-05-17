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

struct Point2D {
    GLshort x, y;

    Point2D();
    Point2D(GLshort x, GLshort y);
    Point2D(uint32_t position);
    static Point2D forTexturePosition(uint16_t position);
    static Point2D forTexturePage(uint32_t texturePage);
    static Point2D forClut(uint16_t clutData);
};

struct Point3D {
    GLshort x, y, z;

    Point3D();
    Point3D(GLshort x, GLshort y, GLshort z);
    Point3D(uint32_t position);
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
    Point3D point;
    Color color;
    Point2D texturePosition;
    GLuint textureBlendMode;
    Point2D texturePage;
    GLuint textureDepthShift;
    Point2D clut;

    Vertex(Point3D point, Color color);
    Vertex(Point3D point, Color color, Point2D texturePosition, TextureBlendMode textureBlendMode, Point2D texturePage, GLuint textureDepthShift, Point2D clut);
    ~Vertex();
};

struct Pixel {
    GLfloat pointX;
    GLfloat pointY;
    GLfloat framebufferPositionX;
    GLfloat framebufferPositionY;

    Pixel(GLfloat pointX, GLfloat pointY, GLfloat framebufferPositionX, GLfloat framebufferPositionY);
    ~Pixel();
};
