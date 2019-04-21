#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include <array>
#include "RendererProgram.hpp"
#include "VertexArrayObject.hpp"
#include "RendererBuffer.hpp"
#include "Vertex.hpp"

class Renderer {
    SDL_GLContext glContext;
    SDL_Window *window;

    std::unique_ptr<RendererProgram> program;
    std::unique_ptr<VertexArrayObject> vao;

    GLuint offsetUniform;

    std::unique_ptr<RendererBuffer<Vertex>> buffer;
    uint32_t verticesCount;

    void draw();
public:
    Renderer();
    ~Renderer();

    void pushTriangle(std::array<Vertex, 3> vertices);
    void pushQuad(std::array<Vertex, 4> vertices);
    void setDrawingOffset(int16_t x, int16_t y);
    void display();
};
