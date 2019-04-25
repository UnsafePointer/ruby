#version 450 core

uniform sampler2D frame_buffer_texture;

in vec3 color;
in vec2 fragment_texture_point;
flat in uint fragment_texture_blend_mode;
flat in uvec2 fragment_texture_page;
flat in uint fragment_texture_depth_shift;

out vec4 fragment_color;

const uint BLEND_MODE_NO_TEXTURE = 0U;
const uint BLEND_MODE_RAW_TEXTURE = 1U;
const uint BLEND_MODE_TEXTURE_BLEND = 2U;

void main() {
    if (fragment_texture_blend_mode == BLEND_MODE_NO_TEXTURE) {
        fragment_color = vec4(color, 1.0);
    } else {
        uint pixel_per_hw = 1U << fragment_texture_depth_shift;

        uint texel_x = uint(fragment_texture_point.x) & 0xffU;
        uint texel_y = uint(fragment_texture_point.y) & 0xffU;

        uint texel_x_pix = texel_x / pixel_per_hw;

        texel_x_pix += fragment_texture_page.x;
        texel_y += fragment_texture_page.y;
        fragment_color = texelFetch(frame_buffer_texture, ivec2(texel_x_pix, texel_y), 0);
    }
}
