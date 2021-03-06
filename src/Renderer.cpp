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

Renderer::Renderer(std::unique_ptr<Window> &mainWindow, GPU *gpu) : logger(LogLevel::NoLog), opaqueVertices(), transparentVertices(), mainWindow(mainWindow), mode(GL_TRIANGLES), displayAreaStart(), screenResolution({}), drawingAreaTopLeft(), drawingAreaSize({}), renderPolygonOneByOne(false), orderingIndex(0) {
    ConfigurationManager *configurationManager = ConfigurationManager::getInstance();
    resizeToFitFramebuffer = configurationManager->shouldResizeWindowToFitFramebuffer();

    textureRendererProgram = make_unique<RendererProgram>("./glsl/texture_load_vertex.glsl", "./glsl/texture_load_fragment.glsl");

    textureBuffer = make_unique<RendererBuffer<Point2D>>(textureRendererProgram, RENDERER_BUFFER_SIZE);

    program = make_unique<RendererProgram>("glsl/vertex.glsl", "glsl/fragment.glsl");
    program->useProgram();

    buffer = make_unique<RendererBuffer<Vertex>>(program, RENDERER_BUFFER_SIZE);

    offsetUniform = program->findProgramUniform("offset");
    glUniform2i(offsetUniform, 0, 0);

    drawTransparentTextureBlendUniform = program->findProgramUniform("draw_transparent_texture_blend");
    glUniform1ui(drawTransparentTextureBlendUniform, 0);

    Dimensions screenDimensions = mainWindow->getDimensions();

    string screenVertexFile = "./glsl/screen_vertex.glsl";
    screenRendererProgram = make_unique<RendererProgram>(screenVertexFile, "./glsl/screen_fragment.glsl");
    screenRendererProgram->useProgram();
    GLuint screenWidthUniform = screenRendererProgram->findProgramUniform("screen_width");
    glUniform1f(screenWidthUniform, screenDimensions.width);
    GLuint screenHeightUniform = screenRendererProgram->findProgramUniform("screen_height");
    glUniform1f(screenHeightUniform, screenDimensions.height);
    GLuint vramWidthUniform = screenRendererProgram->findProgramUniform("vram_width");
    glUniform1f(vramWidthUniform, VRAM_WIDTH);
    GLuint vramHeightUniform = screenRendererProgram->findProgramUniform("vram_height");
    glUniform1f(vramHeightUniform, VRAM_HEIGHT);
    GLuint fullFramebufferUniform = screenRendererProgram->findProgramUniform("full_framebuffer");
    glUniform1f(fullFramebufferUniform, resizeToFitFramebuffer);

    screenBuffer = make_unique<RendererBuffer<Pixel>>(screenRendererProgram, RENDERER_BUFFER_SIZE);

    // TODO: handle resolution for other targets
    loadImageTexture = make_unique<Texture>(((GLsizei) VRAM_WIDTH), ((GLsizei) VRAM_HEIGHT));

    screenTexture = make_unique<Texture>(((GLsizei) screenDimensions.width), ((GLsizei) screenDimensions.height));
    RendererDebugger *rendererDebugger = RendererDebugger::getInstance();

    rendererDebugger->checkForOpenGLErrors();

    displayAreaStart = gpu->getDisplayAreaStart();
    screenResolution = gpu->getResolution();
}

Renderer::~Renderer() {
    SDL_Quit();
}

void Renderer::checkRenderPolygonOneByOne() {
    if (!renderPolygonOneByOne) {
        return;
    }
    prepareFrame();
    renderFrame();
    finalizeFrame();
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

void Renderer::applyScissor() {
    // TODO: there must be a better way of doing this
    // Scissor test is specified in window coordinates, but we get PlayStation screen buffer
    // from the GPU commands, so we need to translate between coordinate systems. The problem here
    // is that we need to handle different modes: full framebuffer and display area cropped.
    Dimensions windowSize = mainWindow->getDimensions();

    // Conveniently the width is the same for both modes, so nothing to do here
    GLsizei width = drawingAreaSize.width;
    GLint x = drawingAreaTopLeft.x;

    GLsizei height = ((float)drawingAreaSize.height / VRAM_HEIGHT * windowSize.height);
    GLint y = windowSize.height - height - ((float)drawingAreaTopLeft.y / VRAM_HEIGHT * windowSize.height);

    if (resizeToFitFramebuffer) {
        height = drawingAreaSize.height;
        y = VRAM_HEIGHT - height - drawingAreaTopLeft.y;
    }
    glScissor(x, y, width, height);
    glEnable(GL_SCISSOR_TEST);
}

void Renderer::insertVertices(std::vector<Vertex> vertices, bool opaque, TextureBlendMode textureBlendMode) {
    bool shouldDrawOpaque = opaque || textureBlendMode != TextureBlendMode::TextureBlendModeNoTexture;
    if (shouldDrawOpaque) {
        opaqueVertices.insert(opaqueVertices.end(), vertices.begin(), vertices.end());
    }
    if (!opaque) {
        transparentVertices.insert(transparentVertices.end(), vertices.begin(), vertices.end());
    }
}

void Renderer::pushLine(std::vector<Vertex> vertices, bool opaque) {
    unsigned int size = vertices.size();
    if (size < 2) {
        logger.logError("Unhandled line with %d vertices", size);
        return;
    }
    checkForceDraw(size, GL_LINES);
    mode = GL_LINES;
    orderingIndex++;
    for (auto& vertix : vertices) {
        vertix.point.z = orderingIndex;
    }
    insertVertices(vertices, opaque, TextureBlendMode::TextureBlendModeNoTexture);
    checkRenderPolygonOneByOne();
    return;
}

void Renderer::pushPolygon(std::vector<Vertex> vertices, bool opaque, TextureBlendMode textureBlendMode) {
    unsigned int size = vertices.size();
    if (size < 3 || size > 4) {
        logger.logError("Unhandled polygon with %d vertices", size);
        return;
    }
    checkForceDraw(size, GL_TRIANGLES);
    mode = GL_TRIANGLES;
    orderingIndex++;
    for (auto& vertix : vertices) {
        vertix.point.z = orderingIndex;
    }
    switch (size) {
        case 3: {
            insertVertices(vertices, opaque, textureBlendMode);
            checkRenderPolygonOneByOne();
            break;
        }
        case 4: {
            insertVertices(vector<Vertex>(vertices.begin(), vertices.end() - 1), opaque, textureBlendMode);
            checkRenderPolygonOneByOne();
            insertVertices(vector<Vertex>(vertices.begin() + 1, vertices.end()), opaque, textureBlendMode);
            checkRenderPolygonOneByOne();
            break;
        }
    }
    return;
}

void Renderer::resetMainWindow() {
    mainWindow->makeCurrent();
}

void Renderer::setDisplayAreaSart(Point2D point) {
    displayAreaStart = point;
}

void Renderer::setScreenResolution(Dimensions dimensions) {
    screenResolution = dimensions;
}

void Renderer::setDrawingArea(Point2D topLeft, Dimensions size) {
    renderFrame();

    drawingAreaTopLeft = topLeft;
    drawingAreaSize = size;

    applyScissor();
}

void Renderer::toggleRenderPolygonOneByOne() {
    renderPolygonOneByOne = !renderPolygonOneByOne;
}

void Renderer::prepareFrame() {
    resetMainWindow();
    applyScissor();
    glEnable(GL_SCISSOR_TEST);
    glBlendColor(0.25, 0.25, 0.25, 0.5);
}

void Renderer::renderFrame() {
    program->useProgram();
    loadImageTexture->bind(GL_TEXTURE0);

    Framebuffer framebuffer = Framebuffer(screenTexture);

    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
    glUniform1ui(drawTransparentTextureBlendUniform, 0);

    buffer->addData(opaqueVertices);
    opaqueVertices.clear();
    buffer->draw(mode);

    glBlendFuncSeparate(GL_CONSTANT_ALPHA, GL_CONSTANT_ALPHA, GL_ONE, GL_ZERO);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glEnable(GL_BLEND);
    glUniform1ui(drawTransparentTextureBlendUniform, 1);

    buffer->addData(transparentVertices);
    transparentVertices.clear();
    buffer->draw(mode);
    orderingIndex = 0;

    RendererDebugger *rendererDebugger = RendererDebugger::getInstance();
    rendererDebugger->checkForOpenGLErrors();
}

void Renderer::finalizeFrame() {
    screenTexture->bind(GL_TEXTURE0);
    glDisable(GL_SCISSOR_TEST);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
    vector<Pixel> pixels;
    if (resizeToFitFramebuffer) {
        pixels = {
            Pixel(-1.0f, -1.0f, 0.0f, 1.0f),
            Pixel(1.0f, -1.0f, 1.0f, 1.0f),
            Pixel(-1.0f, 1.0f, 0.0f, 0.0f),
            Pixel(1.0f, 1.0f, 1.0f, 0.0f),
        };
    } else {
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

void Renderer::updateWindowTitle(string title) {
    SDL_SetWindowTitle(mainWindow->getWindowRef(), title.c_str());
}

void Renderer::setDrawingOffset(int16_t x, int16_t y) {
    renderFrame();
    glUniform2i(offsetUniform, ((GLint)x), ((GLint)y));
}

void Renderer::loadImage(std::unique_ptr<GPUImageBuffer> &imageBuffer) {
    loadImageTexture->setImageFromBuffer(imageBuffer);
    textureBuffer->clean();
    uint16_t x, y, width, height;
    tie(x, y) = imageBuffer->destination();
    tie(width, height) = imageBuffer->resolution();
    vector<Point2D> data = { {(GLshort)x, (GLshort)y}, {(GLshort)(x + width), (GLshort)y}, {(GLshort)x, (GLshort)(y + height)}, {(GLshort)(x + width), (GLshort)(y + height)} };
    textureBuffer->addData(data);
    glDisable(GL_SCISSOR_TEST);
    Framebuffer framebuffer = Framebuffer(screenTexture);
    textureBuffer->draw(GL_TRIANGLE_STRIP);
    glEnable(GL_SCISSOR_TEST);
    RendererDebugger *rendererDebugger = RendererDebugger::getInstance();
    rendererDebugger->checkForOpenGLErrors();
}
