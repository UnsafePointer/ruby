#version 450 core

uniform sampler2D frame_buffer_texture;

in vec3 color;
in vec2 fragment_texture_point;
flat in uint fragment_texture_blend_mode;
flat in uvec2 fragment_texture_page;
flat in uint fragment_texture_depth_shift;
flat in uvec2 fragment_clut;

out vec4 fragment_color;

const uint BLEND_MODE_NO_TEXTURE = 0U;
const uint BLEND_MODE_RAW_TEXTURE = 1U;
const uint BLEND_MODE_TEXTURE_BLEND = 2U;

int ps_color(vec4 color) {
  int a = int(floor(color.a + 0.5));
  int r = int(floor(color.r * 31. + 0.5));
  int g = int(floor(color.g * 31. + 0.5));
  int b = int(floor(color.b * 31. + 0.5));

  return (a << 15) | (b << 10) | (g << 5) | r;
}

bool is_transparent(vec4 texel) {
  return ps_color(texel) == 0U;
}

vec4 get_pixel_from_vram(uint x, uint y) {
  return texelFetch(frame_buffer_texture, ivec2(x & 0x3ffU, y & 0x1ffU), 0);
}

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
        vec4 texel = get_pixel_from_vram(texel_x_pix, texel_y);

        if (fragment_texture_depth_shift > 0) {
            uint align = texel_x & ((1U << fragment_texture_depth_shift) - 1U);
            uint bpp = 16U >> fragment_texture_depth_shift;
            uint shift = (align * bpp);
            uint mask = ((1U << bpp) - 1U);

            uint pscolor = ps_color(texel);
            uint index = (pscolor >> shift) & mask;

            uint clut_x = fragment_clut.x + index;
            uint clut_y = fragment_clut.y;

            texel = get_pixel_from_vram(clut_x, clut_y);
        }

        if (is_transparent(texel)) {
            discard;
        }

        fragment_color = texel;
    }
}
