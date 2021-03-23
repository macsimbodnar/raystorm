#include <math.h>
#include "log.h"
#include "raystorm.h"


typedef struct {
    i32 x;
    i32 y;
    i32 width;
    i32 height;
} rect_t;


// internal i32 round_f32_to_i32(f32 r32) {
//     i32 result = (i32) roundf(r32);
//     return result;
// }


internal u32 round_f32_to_u32(f32 r32) {
    u32 result = (u32) roundf(r32);
    return result;
}


internal void draw_rectangle(game_offscreen_buffer_t *buffer, rect_t *rect, f32 R, f32 G, f32 B) {
    i32 min_x = rect->x;
    i32 min_y = rect->y;
    i32 max_x = rect->x + rect->width;
    i32 max_y = rect->y + rect->height;

    if (min_x < 0) {
        min_x = 0;
    }

    if (min_y < 0) {
        min_y = 0;
    }

    if (max_x > buffer->width) {
        max_x = buffer->width;
    }

    if (max_y > buffer->height) {
        max_y = buffer->height;
    }

    u32 color = ((round_f32_to_u32(R * 255.0f) << 16) | (round_f32_to_u32(G * 255.0f) << 8) | (round_f32_to_u32(B * 255.0f) << 0));

    u8 *row = (((u8 *) buffer->memory) + min_x * buffer->bytes_per_pixel + min_y * buffer->pitch);

    for (int y = min_y; y < max_y; y++) {
        u32 *pixel = (u32 *) row;

        for (int x = min_x; x < max_x; x++) {
            *pixel++ = color;
        }

        row += buffer->pitch;
    }
}


void game_output_sound(game_state_t *game_state, game_sound_output_buffer_t *buffer, int tone_Hz) {
    // i16 toneVolume = 3000;
    // f32 wavePeriod = (f32)buffer->samples_per_second / tone_Hz;

    i16 *sample_out = buffer->samples;

    for (int i = 0; i < buffer->sample_count; i++) {
        i16 sample_value = 1;
        *sample_out++ = sample_value;
        *sample_out++ = sample_value;
    }
}


// void game_update_and_render(game_memory_t *memory, game_input_t *input, game_offscreen_buffer_t *buffer)
GAME_UPDATE_AND_RENDER(game_update_and_render) {
    
    i32 half_height = buffer->height / 2;
    rect_t top = {0, 0,  buffer->width, half_height + 1};
    draw_rectangle(buffer, &top, 1.0f, .0f, .0f);

    rect_t bottom = {0, half_height,  buffer->width,  half_height};
    draw_rectangle(buffer, &bottom, .0f, 1.0f, .0f);
}


// void game_get_sound_samples(game_memory_t *memory, game_sound_output_buffer_t *sound_buffer)
GAME_GET_SOUND_SAMPLES(game_get_sound_samples) {
    game_state_t *game_state = (game_state_t *) memory->permanent_storage;
    game_output_sound(game_state, sound_buffer, 400);
}