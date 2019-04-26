#version 450 core

uniform sampler2D frame_buffer_texture;

in vec2 fragment_texture_position;

out vec4 fragment_color;

void main() {
  fragment_color = texelFetch(frame_buffer_texture, ivec2(fragment_texture_position), 0);
}
