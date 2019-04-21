#pragma once
#include <glad/glad.h>

class VertexArrayObject {
    GLuint object;
public:
    VertexArrayObject();
    ~VertexArrayObject();

    void bind() const;
};
