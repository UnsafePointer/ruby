#pragma once
#include <glad/glad.h>

const uint32_t RENDERER_BUFFER_SIZE = 64*1024;

template <class T>
class RendererBuffer {
    GLuint object;
    T *map;
public:
    RendererBuffer();
    ~RendererBuffer();

    void set(uint32_t index, T value);
    void mapMemory();
    void unmapMemory();
};
