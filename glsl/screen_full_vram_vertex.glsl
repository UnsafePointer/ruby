#version 450 core

in vec2 vertex_point;
in vec2 frame_buffer_point;

out vec2 fragment_frame_buffer_point;

void main() {
    gl_Position = vec4(vertex_point.x, vertex_point.y, 0.0, 1.0);
    float frame_buffer_point_x = float(frame_buffer_point.x) - 1.0;
    float frame_buffer_point_y = 1.0 - float(frame_buffer_point.y);
    fragment_frame_buffer_point = vec2(frame_buffer_point_x, frame_buffer_point_y);
}
