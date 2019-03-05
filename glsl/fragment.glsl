#version 450 core

in vec3 color;
out vec4 fragment_color;

void main() {
    fragment_color = vec4(color, 1.0);
}
