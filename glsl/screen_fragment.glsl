#version 450 core

in vec2 fragment_frame_buffer_point;

uniform sampler2D frame_buffer_texture;

out vec4 fragment_color;

void main() {
    fragment_color = texture(frame_buffer_texture, fragment_frame_buffer_point);
}
