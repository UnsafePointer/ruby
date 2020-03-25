#include "Renderer.hpp"
#include <glad/glad.h>
#include <fstream>
#include <streambuf>
#include <vector>
#include "RendererDebugger.hpp"
#include "Framebuffer.hpp"
#include "GPU.hpp"
#include "ConfigurationManager.hpp"

using namespace std;

Renderer::Renderer(std::unique_ptr<Window> &mainWindow) : logger(LogLevel::NoLog), mainWindow(mainWindow), mode(GL_TRIANGLES) {
    ConfigurationManager *configurationManager = ConfigurationManager::getInstance();
    resizeToFitFramebuffer = configurationManager->shouldResizeWindowToFitFramebuffer();

    textureRendererProgram = make_unique<RendererProgram>("./glsl/texture_load_vertex.glsl", "./glsl/texture_load_fragment.glsl");

    textureBuffer = make_unique<RendererBuffer<Point>>(textureRendererProgram, RENDERER_BUFFER_SIZE);

    program = make_unique<RendererProgram>("glsl/vertex.glsl", "glsl/fragment.glsl");
    program->useProgram();

    buffer = make_unique<RendererBuffer<Vertex>>(program, RENDERER_BUFFER_SIZE);

    offsetUniform = program->findProgramAttribute("offset");
    glUniform2i(offsetUniform, 0, 0);

    // TODO: Use a single vertex shader
    string screenVertexFile = "./glsl/screen_vertex.glsl";
    if (resizeToFitFramebuffer) {
        screenVertexFile = "./glsl/screen_full_vram_vertex.glsl";
    }
    screenRendererProgram = make_unique<RendererProgram>(screenVertexFile, "./glsl/screen_fragment.glsl");

    screenBuffer = make_unique<RendererBuffer<Pixel>>(screenRendererProgram, RENDERER_BUFFER_SIZE);

    // TODO: handle resolution for other targets
    loadImageTexture = make_unique<Texture>(((GLsizei) VRAM_WIDTH), ((GLsizei) VRAM_HEIGHT));

    Dimensions screenDimensions = mainWindow->getDimensions();
    screenTexture = make_unique<Texture>(((GLsizei) screenDimensions.width), ((GLsizei) screenDimensions.height));
    RendererDebugger *rendererDebugger = RendererDebugger::getInstance();
    rendererDebugger->checkForOpenGLErrors();
}

Renderer::~Renderer() {
    SDL_Quit();
}

void Renderer::checkForceDraw(unsigned int verticesToRender, GLenum newMode) {
    unsigned int verticesToRenderTotal = verticesToRender;
    if (verticesToRender == 4) {
        verticesToRenderTotal = 6;
    }
    if (buffer->remainingCapacity() < verticesToRenderTotal) {
        renderFrame();
    }
    if (mode != newMode) {
        renderFrame();
    }
    return;
}

void Renderer::pushLine(std::vector<Vertex> vertices) {
    unsigned int size = vertices.size();
    if (size < 2) {
        logger.logError("Unhandled line with %d vertices", size);
        return;
    }
    checkForceDraw(size, GL_LINES);
    mode = GL_LINES;
    buffer->addData(vertices);
    return;
}

void Renderer::pushPolygon(std::vector<Vertex> vertices) {
    unsigned int size = vertices.size();
    if (size < 3 || size > 4) {
        logger.logError("Unhandled polygon with %d vertices", size);
        return;
    }
    checkForceDraw(size, GL_TRIANGLES);
    mode = GL_TRIANGLES;
    switch (size) {
        case 3: {
            buffer->addData(vertices);
            break;
        }
        case 4: {
            buffer->addData(vector<Vertex>(vertices.begin(), vertices.end() - 1));
            buffer->addData(vector<Vertex>(vertices.begin() + 1, vertices.end()));
            break;
        }
    }
    return;
}

void Renderer::resetMainWindow() {
    mainWindow->makeCurrent();
}

void Renderer::prepareFrame() {
    resetMainWindow();
    loadImageTexture->bind(GL_TEXTURE0);
}

void Renderer::renderFrame() {
    Framebuffer framebuffer = Framebuffer(screenTexture);
    buffer->draw(mode);
    RendererDebugger *rendererDebugger = RendererDebugger::getInstance();
    rendererDebugger->checkForOpenGLErrors();
}

void Renderer::finalizeFrame(GPU *gpu) {
    buffer->draw(mode);
    screenTexture->bind(GL_TEXTURE0);
    vector<Pixel> pixels;
    if (resizeToFitFramebuffer) {
        pixels = {
            Pixel(-1.0f, -1.0f, 0.0f, 1.0f),
            Pixel(1.0f, -1.0f, 1.0f, 1.0f),
            Pixel(-1.0f, 1.0f, 0.0f, 0.0f),
            Pixel(1.0f, 1.0f, 1.0f, 0.0f),
        };
    } else {
        Point displayAreaStart = gpu->getDisplayAreaStart();
        Dimensions screenResolution = gpu->getResolution();
        pixels = {
            Pixel(-1.0f, -1.0f, displayAreaStart.x, displayAreaStart.y + screenResolution.height),
            Pixel(1.0f, -1.0f, displayAreaStart.x + screenResolution.width, displayAreaStart.y + screenResolution.height),
            Pixel(-1.0f, 1.0f, displayAreaStart.x, displayAreaStart.y),
            Pixel(1.0f, 1.0f, displayAreaStart.x + screenResolution.width, displayAreaStart.y),
        };
    }
    screenBuffer->addData(pixels);
    screenBuffer->draw(GL_TRIANGLE_STRIP);
    RendererDebugger *rendererDebugger = RendererDebugger::getInstance();
    rendererDebugger->checkForOpenGLErrors();
    SDL_GL_SwapWindow(mainWindow->getWindowRef());
}

void Renderer::setDrawingOffset(int16_t x, int16_t y) {
    buffer->draw(mode);
    glUniform2i(offsetUniform, ((GLint)x), ((GLint)y));
}

void Renderer::loadImage(std::unique_ptr<GPUImageBuffer> &imageBuffer) {
    loadImageTexture->setImageFromBuffer(imageBuffer);
    textureBuffer->clean();
    uint16_t x, y, width, height;
    tie(x, y) = imageBuffer->destination();
    tie(width, height) = imageBuffer->resolution();
    vector<Point> data = { {(GLshort)x, (GLshort)y}, {(GLshort)(x + width), (GLshort)y}, {(GLshort)x, (GLshort)(y + height)}, {(GLshort)(x + width), (GLshort)(y + height)} };
    textureBuffer->addData(data);
    Framebuffer framebuffer = Framebuffer(screenTexture);
    textureBuffer->draw(GL_TRIANGLE_STRIP);
    RendererDebugger *rendererDebugger = RendererDebugger::getInstance();
    rendererDebugger->checkForOpenGLErrors();
}
