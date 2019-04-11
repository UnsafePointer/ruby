#include "Texture.hpp"
#include <iostream>

using namespace std;

Texture::Texture(GLsizei width, GLsizei height) {
    glGenTextures(1, &object);
    glBindTexture(GL_TEXTURE_2D, object);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB5_A1, width, height);
}

Texture::~Texture() {
    glDeleteTextures(1, &object);
}

void Texture::setImageFromBuffer(std::unique_ptr<GPUImageBuffer> &imageBuffer) {
    if (!imageBuffer->isValid()) {
        cout << "Invalid image buffer" << endl;
        exit(1);
    }
    uint16_t x, y, width, height;
    tie(x, y) = imageBuffer->destination();
    tie(width, height) = imageBuffer->resolution();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, object);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, imageBuffer->bufferRef());
}
