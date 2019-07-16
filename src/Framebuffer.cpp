#include "Framebuffer.hpp"

Framebuffer::Framebuffer(std::unique_ptr<Texture> &texture) {
    glGenFramebuffers(1, &object);
    glBindFramebuffer(GL_FRAMEBUFFER, object);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getID(), 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, texture->getWidth(), texture->getHeight());
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &object);
}
