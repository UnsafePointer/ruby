#pragma once
#include <glad/glad.h>
#include <memory>
#include <vector>
#include "VertexArrayObject.hpp"
#include "RendererProgram.hpp"

const uint32_t RENDERER_BUFFER_SIZE = 64*1024;

template <class T>
class RendererBuffer {
    std::unique_ptr<VertexArrayObject> vao;
    GLuint vbo;
    std::unique_ptr<RendererProgram> &program;
    unsigned int capacity;
    unsigned int size;

    void enableAttributes() const;
public:
    RendererBuffer(std::unique_ptr<RendererProgram> &program, unsigned int capacity);
    ~RendererBuffer();

    void bind() const;
    void clean();
    void draw(GLenum mode);
    void addData(std::vector<T> data);
    unsigned int remainingCapacity();
};

