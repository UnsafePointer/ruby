#include "RendererBuffer.hpp"
#include <stddef.h>
#include <iostream>
#include "Vertex.hpp"
#include "RendererDebugger.hpp"

using namespace std;

template <class T>
RendererBuffer<T>::RendererBuffer(unique_ptr<RendererProgram> &program, uint capacity) : vao(make_unique<VertexArrayObject>()), program(program), capacity(capacity), size(0) {
    glGenBuffers(1, &vbo);

    vao->bind();
    bind();

    GLsizeiptr bufferSize = sizeof(T) * capacity;
    glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
    enableAttributes();
}

template <class T>
RendererBuffer<T>::~RendererBuffer() {
    glDeleteBuffers(1, &vbo);
}

template <class T>
void RendererBuffer<T>::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

template <class T>
void RendererBuffer<T>::clean() {
    bind();
    GLsizeiptr bufferSize = sizeof(T) * capacity;
    glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
    size = 0;
}

template <class T>
void RendererBuffer<T>::draw(GLenum mode) {
    program->useProgram();
    glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
    glDrawArrays(mode, 0, (GLsizei)size);
    GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    while (true) {
        GLenum result = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 10000000);
        if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
            break;
        }
    }
    clean();
    checkForOpenGLErrors();
}

template <class T>
void RendererBuffer<T>::addData(vector<T> data) {
    uint remainingCapacity = capacity - size;
    if (data.size() > remainingCapacity) {
        cout << "Renderer buffer out of memory." << endl;
        exit(1);
    }
    bind();

    uint offset = size * sizeof(T);
    uint dataSize = data.size() * sizeof(T);
    glBufferSubData(GL_ARRAY_BUFFER, offset, dataSize, data.data());

    size += data.size();
}

template <>
void RendererBuffer<Vertex>::enableAttributes() const {
    GLuint positionIdx = program->findProgramAttribute("vertex_point");
    glVertexAttribIPointer(positionIdx, 2, GL_SHORT, sizeof(Vertex), (void*)offsetof(struct Vertex, position));
    glEnableVertexAttribArray(positionIdx);

    GLuint colorIdx = program->findProgramAttribute("vertex_color");
    glVertexAttribIPointer(colorIdx, 3, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(struct Vertex, color));
    glEnableVertexAttribArray(colorIdx);
}

template <>
void RendererBuffer<Point>::enableAttributes() const {
    GLuint positionIdx = program->findProgramAttribute("position");
    glVertexAttribIPointer(positionIdx, 2, GL_SHORT, 0, NULL);
    glEnableVertexAttribArray(positionIdx);
}

template class RendererBuffer<Vertex>;
template class RendererBuffer<Point>;
