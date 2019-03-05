#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include "Point.hpp"
#include "Color.hpp"
#include "RendererBuffer.hpp"

class Renderer {
    SDL_GLContext glContext;
    SDL_Window *window;

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint glProgram;
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

    void pushTriangle(Point (&points)[3], Color (&colors)[3]);
    void display();
};
