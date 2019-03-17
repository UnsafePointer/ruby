#include "RendererBuffer.hpp"
#include <iostream>
#include "Point.hpp"
#include "Color.hpp"

using namespace std;

template <class T>
RendererBuffer<T>::RendererBuffer() {
    glGenBuffers(1, &object);
    mapMemory();
}

template <class T>
RendererBuffer<T>::~RendererBuffer() {
    unmapMemory();
    glDeleteBuffers(1, &object);
}

template <class T>
void RendererBuffer<T>::set(uint32_t index, T value) {
    if (index >= RENDERER_BUFFER_SIZE) {
        cout << "Renderer Buffer overflow" << endl;
    }

    map[index] = value;
}

template <class T>
void RendererBuffer<T>::mapMemory() {
    glBindBuffer(GL_ARRAY_BUFFER, object);

    GLsizeiptr bufferElementSize = sizeof(T);
    GLsizeiptr bufferSize = bufferElementSize * RENDERER_BUFFER_SIZE;

#ifndef GAMESHELL
    GLbitfield accessFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
    glBufferStorage(GL_ARRAY_BUFFER, bufferSize, nullptr, accessFlags);
    map = (T*)glMapBufferRange(GL_ARRAY_BUFFER, 0, bufferSize, accessFlags);
#else
    GLenum usageFlags = GL_DYNAMIC_DRAW;
    GLbitfield accessFlags = GL_MAP_WRITE_BIT;
    glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, usageFlags);
    map = (T*)glMapBufferRange(GL_ARRAY_BUFFER, 0, bufferSize, accessFlags);
#endif
}

template <class T>
void RendererBuffer<T>::unmapMemory() {
    glBindBuffer(GL_ARRAY_BUFFER, object);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

template class RendererBuffer<Point>;
template class RendererBuffer<Color>;
