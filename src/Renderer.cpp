#include "Renderer.hpp"
#include <iostream>
#include <glad/glad.h>
#include <fstream>
#include <streambuf>
#include <vector>
#include "RendererDebugger.hpp"

using namespace std;

Renderer::Renderer() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "Error initializing SDL: " << SDL_GetError() << endl;
        exit(1);
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    int width = 1024;
    int height = 512;
    window = SDL_CreateWindow("ルビィ", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    glContext = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        cout << "Failed to initialize the OpenGL context." << std::endl;
        exit(1);
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(window);

    program = make_unique<RendererProgram>("glsl/vertex.glsl", "glsl/fragment.glsl");
    program->useProgram();

    buffer = make_unique<RendererBuffer<Vertex>>(program, RENDERER_BUFFER_SIZE);

    offsetUniform = program->findProgramAttribute("offset");
    glUniform2i(offsetUniform, 0, 0);

    // TODO: handle resolution for other targets
    frameBufferTexture = make_unique<Texture>(((GLsizei) width), ((GLsizei) height));
    checkForOpenGLErrors();
}

Renderer::~Renderer() {
    SDL_Quit();
}

void Renderer::pushTriangle(std::array<Vertex, 3> vertices) {
    buffer->addData(vector<Vertex>(vertices.begin(), vertices.end()));
    return;
}


void Renderer::pushQuad(std::array<Vertex, 4> vertices) {
    buffer->addData(vector<Vertex>(vertices.begin(), vertices.end() - 1));
    buffer->addData(vector<Vertex>(vertices.begin() + 1, vertices.end()));
    return;
}

void Renderer::display() {
    buffer->draw();
    SDL_GL_SwapWindow(window);
}

void Renderer::setDrawingOffset(int16_t x, int16_t y) {
    buffer->draw();
    glUniform2i(offsetUniform, ((GLint)x), ((GLint)y));
}

void Renderer::loadImage(std::unique_ptr<GPUImageBuffer> &imageBuffer) {
    frameBufferTexture->setImageFromBuffer(imageBuffer);
}
