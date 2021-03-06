#version 450 core

in ivec3 vertex_point;
in uvec3 vertex_color;
in uint transparent;
in ivec2 texture_point;
in uint texture_blend_mode;
in uvec2 texture_page;
in uint texture_depth_shift;
in uvec2 clut;

out vec3 color;
flat out uint fragment_transparent;
out vec2 fragment_texture_point;
flat out uint fragment_texture_blend_mode;
flat out uvec2 fragment_texture_page;
flat out uint fragment_texture_depth_shift;
flat out uvec2 fragment_clut;

uniform ivec2 offset;

void main() {
    ivec2 position = vertex_point.xy + offset;

    float x_pos = (float(position.x) / 512) - 1.0;
    float y_pos = 1.0 - (float(position.y) / 256);
    float z_pos = 1.0 - (float(vertex_point.z) / 32768);

    gl_Position.xyzw = vec4(x_pos, y_pos, z_pos, 1.0);
    color = vec3(float(vertex_color.r) / 255, float(vertex_color.g) / 255, float(vertex_color.b) / 255);
    fragment_texture_point = vec2(texture_point);
    fragment_texture_blend_mode = texture_blend_mode;
    fragment_texture_page = texture_page;
    fragment_texture_depth_shift = texture_depth_shift;
    fragment_clut = clut;
    fragment_transparent = transparent;
}
