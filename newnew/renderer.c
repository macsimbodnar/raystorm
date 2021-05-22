#include "renderer.h"
#include "intrinsics.h"


void draw_rectangle(game_offscreen_buffer_t *buffer, const rect_t rect, const color_t color) {
    i32 min_x = rect.x;
    i32 min_y = rect.y;
    i32 max_x = rect.x + rect.width;
    i32 max_y = rect.y + rect.height;

    // *INDENT-OFF*
    if (min_x < 0)                  min_x = 0;
    if (min_y < 0)                  min_y = 0;
    if (max_x > buffer->width)      max_x = buffer->width;
    if (max_y > buffer->height)     max_y = buffer->height;
    // *INDENT-ON*

    u32 color_32 = ((round_f32_to_u32(color.R * 255.0f) << 16) | (round_f32_to_u32(color.G * 255.0f) << 8) | (round_f32_to_u32(color.B * 255.0f) << 0));

    u8 *row = (((u8 *) buffer->memory) + min_x * buffer->bytes_per_pixel + min_y * buffer->pitch);

    for (int y = min_y; y < max_y; y++) {
        u32 *pixel = (u32 *) row;

        for (int x = min_x; x < max_x; x++) {
            *pixel++ = color_32;
        }

        row += buffer->pitch;
    }
}


void draw_world(game_offscreen_buffer_t *buffer, const world_t *world) {
    
}