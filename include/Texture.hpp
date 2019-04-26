#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <memory>
#include "GPUImageBuffer.hpp"

class Texture {
    GLuint object;
    GLsizei width;
    GLsizei height;
public:
    Texture(GLsizei width, GLsizei height);
    ~Texture();

    void setImageFromBuffer(std::unique_ptr<GPUImageBuffer> &imageBuffer);
};
