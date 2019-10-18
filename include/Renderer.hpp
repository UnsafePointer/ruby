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
#include "Window.hpp"
#include "Logger.hpp"

class GPU;

class Renderer {
    Logger logger;
    GLuint offsetUniform;

    std::unique_ptr<RendererProgram> program;
    std::unique_ptr<RendererBuffer<Vertex>> buffer;

    std::unique_ptr<Texture> loadImageTexture;
    std::unique_ptr<RendererProgram> textureRendererProgram;
    std::unique_ptr<RendererBuffer<Point>> textureBuffer;

    std::unique_ptr<Texture> screenTexture;
    std::unique_ptr<RendererProgram> screenRendererProgram;
    std::unique_ptr<RendererBuffer<Pixel>> screenBuffer;

    GLenum mode;
    bool resizeToFitFramebuffer;

    void checkForceDraw(uint verticesToRender, GLenum newMode);
public:
    Renderer(std::unique_ptr<Window> &mainWindow);
    ~Renderer();

    void pushLine(std::vector<Vertex> vertices);
    void pushPolygon(std::vector<Vertex> vertices);
    void setDrawingOffset(int16_t x, int16_t y);
    void prepareFrame();
    void renderFrame();
    void finalizeFrame(GPU *gpu);
    void loadImage(std::unique_ptr<GPUImageBuffer> &imageBuffer);
};
