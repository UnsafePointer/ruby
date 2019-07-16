#include "Renderer.hpp"
#include <glad/glad.h>
#include <fstream>
#include <streambuf>
#include <vector>
#include "RendererDebugger.hpp"
#include "Output.hpp"

using namespace std;

Renderer::Renderer() : mode(GL_TRIANGLES) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printError("Error initializing SDL: %s", SDL_GetError());
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    window = SDL_CreateWindow("ルビィ", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, VRAM_WIDTH, VRAM_HEIGHT, SDL_WINDOW_OPENGL);
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

    // TODO: handle resolution for other targets
    loadImageTexture = make_unique<Texture>(((GLsizei) VRAM_WIDTH), ((GLsizei) VRAM_HEIGHT));
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
        display();
    }
    if (mode != newMode) {
        display();
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

void Renderer::display() {
    buffer->draw(mode);
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
    textureBuffer->draw(GL_TRIANGLE_STRIP);
}
