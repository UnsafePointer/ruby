#include "Texture.hpp"
#include "RendererDebugger.hpp"
#include <iostream>

using namespace std;

Texture::Texture(GLsizei width, GLsizei height) : logger(LogLevel::NoLog), width(width), height(height) {
    glGenTextures(1, &object);
    glBindTexture(GL_TEXTURE_2D, object);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB5_A1, width, height);
}

Texture::~Texture() {
    glDeleteTextures(1, &object);
}

GLuint Texture::getID() {
    return object;
}

GLsizei Texture::getWidth() {
    return width;
}

GLsizei Texture::getHeight() {
    return height;
}

void Texture::bind(GLenum texture) {
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, object);
}

void Texture::setImageFromBuffer(std::unique_ptr<GPUImageBuffer> &imageBuffer) {
    if (!imageBuffer->isValid()) {
        logger.logError("Invalid image buffer");
    }
    uint16_t x, y, width, height;
    tie(x, y) = imageBuffer->destination();
    tie(width, height) = imageBuffer->resolution();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, object);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, imageBuffer->bufferRef());
    RendererDebugger *rendererDebugger = RendererDebugger::getInstance();
    rendererDebugger->checkForOpenGLErrors();
}
