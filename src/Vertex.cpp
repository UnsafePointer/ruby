#include "Vertex.hpp"

Vertex::Vertex(uint32_t position, uint32_t color) {
    GLshort x = ((GLshort)(position & 0xffff));
    GLshort y = ((GLshort)((position >> 16) & 0xffff));
    this->position = {x, y};
    GLubyte red = ((GLubyte)(color & 0xff));
    GLubyte green = ((GLubyte)((color >> 8) & 0xff));
    GLubyte blue = ((GLubyte)((color >> 16) & 0xff));
    this->color = {red, green, blue};
}

Vertex::~Vertex() {

}
