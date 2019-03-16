#version 450 core

in ivec2 vertex_point;
in uvec3 vertex_color;

out vec3 color;

uniform ivec2 offset;

void main() {
    ivec2 position = vertex_point + offset;

    float x_pos = (float(position.x) / 320) - 1.0;
    float y_pos = 1.0 - (float(position.y) / 240);

    gl_Position.xyzw = vec4(x_pos, y_pos, 0.0, 1.0);
    color = vec3(float(vertex_color.r) / 255, float(vertex_color.g) / 255, float(vertex_color.b) / 255);
}
