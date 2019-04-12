#version 450 core

in uvec2 position;

out vec2 fragment_texture_position;

void main() {
    float x_pos = (float(position.x) / 512) - 1.0;
    float y_pos = 1.0 - (float(position.y) / 256);

    gl_Position.xyzw = vec4(x_pos, y_pos, 0.0, 1.0);
    fragment_texture_position = vec2(position);
}
