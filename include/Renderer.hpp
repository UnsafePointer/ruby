#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include <array>
#include "Point.hpp"
#include "Color.hpp"
#include "RendererBuffer.hpp"

class Renderer {
    SDL_GLContext glContext;
    SDL_Window *window;

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint glProgram;
    GLuint offsetUniform;
    GLuint vertexArrayObject;
    std::unique_ptr<RendererBuffer<Point>> pointsBuffer;
    std::unique_ptr<RendererBuffer<Color>> colorsBuffer;
    uint32_t verticesCount;

    std::string openShaderSource(std::string filePath) const;
    GLuint compileShader(std::string source, GLenum shaderType) const;
    GLuint linkProgram() const;
    GLuint findProgramAttribute(std::string attribute) const;
    void draw();
public:
    Renderer();
    ~Renderer();

    void pushTriangle(std::array<Point, 3> points, std::array<Color, 3> colors);
    void pushQuad(std::array<Point, 4> points, std::array<Color, 4> colors);
    void setDrawingOffset(int16_t x, int16_t y);
    void display();
};
