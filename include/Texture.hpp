#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <memory>
#include "GPUImageBuffer.hpp"
#include "Logger.hpp"

class Texture {
    Logger logger;
    GLuint object;
    GLsizei width;
    GLsizei height;
public:
    Texture(GLsizei width, GLsizei height);
    ~Texture();

    GLuint getID();
    GLsizei getWidth();
    GLsizei getHeight();
    void bind(GLenum texture);
    void setImageFromBuffer(std::unique_ptr<GPUImageBuffer> &imageBuffer);
};
