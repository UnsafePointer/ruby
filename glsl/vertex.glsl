#version 450 core

in ivec2 vertex_point;
in uvec3 vertex_color;

out vec3 color;

void main() {
    float x_pos = (float(vertex_point.x) / 512) - 1.0;
    float y_pos = 1.0 - (float(vertex_point.y) / 256);

    gl_Position.xyzw = vec4(x_pos, y_pos, 0.0, 1.0);
    color = vec3(float(vertex_color.r) / 255, float(vertex_color.g) / 255, float(vertex_color.b) / 255);
}
