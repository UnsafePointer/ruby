#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include <array>
#include "Point.hpp"
#include "Color.hpp"
#include "RendererProgram.hpp"
#include "VertexArrayObject.hpp"
#include "RendererBuffer.hpp"

class Renderer {
    SDL_GLContext glContext;
    SDL_Window *window;

    std::unique_ptr<RendererProgram> program;
    std::unique_ptr<VertexArrayObject> vao;

    GLuint offsetUniform;

    std::unique_ptr<RendererBuffer<Point>> pointsBuffer;
    std::unique_ptr<RendererBuffer<Color>> colorsBuffer;
    uint32_t verticesCount;

    void draw();
public:
    Renderer();
    ~Renderer();

    void pushTriangle(std::array<Point, 3> points, std::array<Color, 3> colors);
    void pushQuad(std::array<Point, 4> points, std::array<Color, 4> colors);
    void setDrawingOffset(int16_t x, int16_t y);
    void display();
};
