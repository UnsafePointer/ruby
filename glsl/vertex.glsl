#version 450 core

in ivec2 vertex_point;
in uvec3 vertex_color;
in ivec2 texture_point;
in uint texture_blend_mode;
in uvec2 texture_page;

out vec3 color;
out vec2 fragment_texture_point;
flat out uint fragment_texture_blend_mode;
flat out uvec2 fragment_texture_page;

uniform ivec2 offset;

void main() {
    ivec2 position = vertex_point + offset;

    float x_pos = (float(position.x) / 512) - 1.0;
    float y_pos = 1.0 - (float(position.y) / 256);

    gl_Position.xyzw = vec4(x_pos, y_pos, 0.0, 1.0);
    color = vec3(float(vertex_color.r) / 255, float(vertex_color.g) / 255, float(vertex_color.b) / 255);
    fragment_texture_point = vec2(texture_point);
    fragment_texture_blend_mode = texture_blend_mode;
    fragment_texture_page = texture_page;
}
