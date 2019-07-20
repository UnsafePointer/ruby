#include "DebugInfoRenderer.hpp"
#include <glad/glad.h>

using namespace std;

DebugInfoRenderer::DebugInfoRenderer() {

}

DebugInfoRenderer::~DebugInfoRenderer() {

}

void DebugInfoRenderer::update() {
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
