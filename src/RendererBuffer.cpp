#include "RendererBuffer.hpp"
#include <stddef.h>
#include "Vertex.hpp"
#include "RendererDebugger.hpp"

using namespace std;

template <class T>
RendererBuffer<T>::RendererBuffer(unique_ptr<RendererProgram> &program, unsigned int capacity) : vao(make_unique<VertexArrayObject>()), program(program), capacity(capacity), size(0) {
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
    vao->bind();
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
    RendererDebugger *rendererDebugger = RendererDebugger::getInstance();
    rendererDebugger->checkForOpenGLErrors();
}

template <class T>
void RendererBuffer<T>::addData(vector<T> data) {
    bind();

    unsigned int offset = size * sizeof(T);
    unsigned int dataSize = data.size() * sizeof(T);
    glBufferSubData(GL_ARRAY_BUFFER, offset, dataSize, data.data());

    size += data.size();
}

template <class T>
unsigned int RendererBuffer<T>::remainingCapacity() {
    int remainingCapacity = capacity - size;
    return remainingCapacity;
}

template <>
void RendererBuffer<Vertex>::enableAttributes() const {
    GLuint positionIdx = program->findProgramAttribute("vertex_point");
    glVertexAttribIPointer(positionIdx, 3, GL_SHORT, sizeof(Vertex), (void*)offsetof(struct Vertex, point));
    glEnableVertexAttribArray(positionIdx);

    GLuint colorIdx = program->findProgramAttribute("vertex_color");
    glVertexAttribIPointer(colorIdx, 3, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(struct Vertex, color));
    glEnableVertexAttribArray(colorIdx);

    GLuint transparentPositionIdx = program->findProgramAttribute("transparent");
    glVertexAttribIPointer(transparentPositionIdx, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(struct Vertex, transparent));
    glEnableVertexAttribArray(transparentPositionIdx);

    GLuint texturePositionIdx = program->findProgramAttribute("texture_point");
    glVertexAttribIPointer(texturePositionIdx, 2, GL_SHORT, sizeof(Vertex), (void*)offsetof(struct Vertex, texturePosition));
    glEnableVertexAttribArray(texturePositionIdx);

    GLuint textureBlendModePositionIdx = program->findProgramAttribute("texture_blend_mode");
    glVertexAttribIPointer(textureBlendModePositionIdx, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(struct Vertex, textureBlendMode));
    glEnableVertexAttribArray(textureBlendModePositionIdx);

    GLuint texturePageIdx = program->findProgramAttribute("texture_page");
    glVertexAttribIPointer(texturePageIdx, 2, GL_SHORT, sizeof(Vertex), (void*)offsetof(struct Vertex, texturePage));
    glEnableVertexAttribArray(texturePageIdx);

    GLuint textureDepthShiftIdx = program->findProgramAttribute("texture_depth_shift");
    glVertexAttribIPointer(textureDepthShiftIdx, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(struct Vertex, textureDepthShift));
    glEnableVertexAttribArray(textureDepthShiftIdx);

    GLuint clutIdx = program->findProgramAttribute("clut");
    glVertexAttribIPointer(clutIdx, 2, GL_SHORT, sizeof(Vertex), (void*)offsetof(struct Vertex, clut));
    glEnableVertexAttribArray(clutIdx);
}

template <>
void RendererBuffer<Point2D>::enableAttributes() const {
    GLuint positionIdx = program->findProgramAttribute("position");
    glVertexAttribIPointer(positionIdx, 2, GL_SHORT, 0, NULL);
    glEnableVertexAttribArray(positionIdx);
}

template <>
void RendererBuffer<Pixel>::enableAttributes() const {
    GLuint positionIdx = program->findProgramAttribute("vertex_point");
    glVertexAttribPointer(positionIdx, 2, GL_FLOAT, GL_FALSE, sizeof(Pixel), (void*)offsetof(struct Pixel, pointX));
    glEnableVertexAttribArray(positionIdx);
    GLuint texturePositionIdx = program->findProgramAttribute("frame_buffer_point");
    glVertexAttribPointer(texturePositionIdx, 2, GL_FLOAT, GL_FALSE, sizeof(Pixel), (void*)offsetof(struct Pixel, framebufferPositionX));
    glEnableVertexAttribArray(texturePositionIdx);
}

template class RendererBuffer<Vertex>;
template class RendererBuffer<Point2D>;
template class RendererBuffer<Pixel>;
