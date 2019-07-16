#pragma once
#include <memory>
#include <glad/glad.h>
#include "Texture.hpp"

class Framebuffer {
    GLuint object;
public:
    Framebuffer(std::unique_ptr<Texture> &texture);
    ~Framebuffer();
};
