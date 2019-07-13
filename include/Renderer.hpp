#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include <vector>
#include "RendererProgram.hpp"
#include "RendererBuffer.hpp"
#include "Vertex.hpp"
#include "GPUImageBuffer.hpp"
#include "Texture.hpp"

class Renderer {
    SDL_GLContext glContext;
    SDL_Window *window;

    std::unique_ptr<RendererProgram> program;

    GLuint offsetUniform;

    std::unique_ptr<RendererBuffer<Vertex>> buffer;

    std::unique_ptr<Texture> frameBufferTexture;
    std::unique_ptr<RendererProgram> textureRendererProgram;
    std::unique_ptr<RendererBuffer<Point>> textureBuffer;

    GLenum mode;
    void checkForceDraw(uint verticesToRender, GLenum newMode);
public:
    Renderer();
    ~Renderer();

    void pushLine(std::vector<Vertex> vertices);
    void pushPolygon(std::vector<Vertex> vertices);
    void setDrawingOffset(int16_t x, int16_t y);
    void display();
    void loadImage(std::unique_ptr<GPUImageBuffer> &imageBuffer);
};
