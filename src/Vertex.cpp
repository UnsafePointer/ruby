#include "Vertex.hpp"
#include "GPU.hpp"

Point2D::Point2D() : x(), y() {}

Point2D::Point2D(GLshort x, GLshort y) : x(x), y(y) {}

Point2D::Point2D(uint32_t position) {
    x = ((GLshort)(position & 0xffff));
    y = ((GLshort)((position >> 16) & 0xffff));
}

Point2D Point2D::forTexturePosition(uint16_t position) {
    GLshort texturePositonX = ((GLshort)(position & 0xff));
    GLshort texturePositionY = ((GLshort)((position >> 8) & 0xff));
    return {texturePositonX, texturePositionY};
}

/*
Texture page:
0-3   Texture page X Base   (N*64) (ie. in 64-halfword steps)    ;GPUSTAT.0-3
4     Texture page Y Base   (N*256) (ie. 0 or 256)               ;GPUSTAT.4
5-6   Semi Transparency     (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4)   ;GPUSTAT.5-6
7-8   Texture page colors   (0=4bit, 1=8bit, 2=15bit, 3=Reserved);GPUSTAT.7-8
*/
Point2D Point2D::forTexturePage(uint32_t texturePage) {
    GLshort texturePageX = ((GLshort)((texturePage & 0xf) << 6));
    GLshort texturePageY = ((GLshort)(((texturePage >> 4) & 0x1) << 8));
    return {texturePageX, texturePageY};
}

/*
CLUT:
0-5      X coordinate X/16  (ie. in 16-halfword steps)
6-14     Y coordinate 0-511 (ie. in 1-line steps)
15       Unknown/unused (should be 0)
*/
Point2D Point2D::forClut(uint16_t clutData) {
    GLshort x = ((GLshort)((clutData & 0x3f) << 4));
    GLshort y = ((GLshort)((clutData >> 6) & 0x1ff));
    return {x, y};
}

Point3D::Point3D() : x(), y(), z() {}

Point3D::Point3D(GLshort x, GLshort y, GLshort z) : x(x), y(y), z(z) {}

Point3D::Point3D(uint32_t position) {
    x = ((GLshort)(position & 0xffff));
    y = ((GLshort)((position >> 16) & 0xffff));
    z = 0;
}

Color::Color(uint32_t color) {
    r = ((GLubyte)(color & 0xff));
    g = ((GLubyte)((color >> 8) & 0xff));
    b = ((GLubyte)((color >> 16) & 0xff));
}

Vertex::Vertex(Point3D point, Color color) : point(point), color(color), texturePosition(), textureBlendMode(), texturePage(), textureDepthShift(), clut() {}

Vertex::Vertex(Point3D point, Color color, Point2D texturePosition, TextureBlendMode textureBlendMode, Point2D texturePage, GLuint textureDepthShift, Point2D clut) : point(point), color(color), texturePosition(texturePosition), textureBlendMode(textureBlendMode), texturePage(texturePage), textureDepthShift(textureDepthShift), clut(clut) {}

Vertex::~Vertex() {}

Pixel::Pixel(GLfloat pointX, GLfloat pointY, GLfloat framebufferPositionX, GLfloat framebufferPositionY) : pointX(pointX), pointY(pointY), framebufferPositionX(framebufferPositionX), framebufferPositionY(framebufferPositionY) {}

Pixel::~Pixel() {}
