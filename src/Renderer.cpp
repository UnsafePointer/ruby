#include "Renderer.hpp"
#include <glad/glad.h>
#include <fstream>
#include <streambuf>
#include <vector>
#include "RendererDebugger.hpp"
#include "Output.hpp"
#include "Framebuffer.hpp"
#include "GPU.hpp"
#include "TestRunner.hpp"

using namespace std;

const uint32_t SCREEN_WIDTH = 1024;
const uint32_t SCREEN_HEIGHT = 768;

Renderer::Renderer() : mode(GL_TRIANGLES) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printError("Error initializing SDL: %s", SDL_GetError());
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    TestRunner *testRunner = TestRunner::getInstance();
    resizeToFitFramebuffer = testRunner->shouldResizeWindowToFitFramebuffer();

    uint32_t screenHeight = SCREEN_HEIGHT;
    if (resizeToFitFramebuffer) {
        screenHeight = 512;
    }

    window = SDL_CreateWindow("ルビィ", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, screenHeight, SDL_WINDOW_OPENGL);
    glContext = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        printError("Failed to initialize the OpenGL context.");
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(window);

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

    screenTexture = make_unique<Texture>(((GLsizei) SCREEN_WIDTH), ((GLsizei) screenHeight));
    checkForOpenGLErrors();
}

Renderer::~Renderer() {
    SDL_Quit();
}

void Renderer::checkForceDraw(uint verticesToRender, GLenum newMode) {
    uint verticesToRenderTotal = verticesToRender;
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
    uint size = vertices.size();
    if (size < 2) {
        printError("Unhandled line with %d vertices", size);
        return;
    }
    checkForceDraw(size, GL_LINES);
    mode = GL_LINES;
    buffer->addData(vertices);
    return;
}

void Renderer::pushPolygon(std::vector<Vertex> vertices) {
    uint size = vertices.size();
    if (size < 3 || size > 4) {
        printError("Unhandled polygon with %d vertices", size);
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

void Renderer::prepareFrame() {
    loadImageTexture->bind(GL_TEXTURE0);
}

void Renderer::renderFrame() {
    Framebuffer framebuffer = Framebuffer(screenTexture);
    buffer->draw(mode);
    checkForOpenGLErrors();
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
    checkForOpenGLErrors();
    SDL_GL_SwapWindow(window);
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
    checkForOpenGLErrors();
}
