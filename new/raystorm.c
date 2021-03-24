#include <math.h>
#include "log.h"
#include "assets.h"
#include "raystorm.h"

#define MINIMAP_W                       150
#define MINIMAP_H                       100
#define MINIMAP_BYTES_PER_PIXEL         4
#define MINIMAP_PIXELS_PER_TILE         10
global_var u8 minimap_mem[MINIMAP_W * MINIMAP_H * MINIMAP_BYTES_PER_PIXEL];
global_var game_offscreen_buffer_t g_minimap = {
    .memory = minimap_mem,
    .width = MINIMAP_W,
    .height = MINIMAP_H,
    .pitch = MINIMAP_W * MINIMAP_BYTES_PER_PIXEL,
    .bytes_per_pixel = MINIMAP_BYTES_PER_PIXEL
};

global_var i32 g_player_x = 60;
global_var i32 g_player_y = 60;


#define MAP_W       24
#define MAP_H       24
u8 map_walls[MAP_W * MAP_H] = {                  // Map of walls 24 * 24
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2,
    2, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2,
    2, 2, 4, 2, 2, 0, 0, 3, 2, 2, 4, 2, 2, 0, 0, 3, 2, 2, 4, 2, 2, 0, 0, 3,
    2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 1, 2, 2, 2, 0, 0, 0, 0, 1, 2, 2, 2, 0, 0, 0, 0, 1, 2, 2,
    2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2,
    2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2,
    2, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2,
    2, 2, 4, 2, 2, 0, 0, 3, 2, 2, 4, 2, 2, 0, 0, 3, 2, 2, 4, 2, 2, 0, 0, 3,
    2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 1, 2, 2, 2, 0, 0, 0, 0, 1, 2, 2, 2, 0, 0, 0, 0, 1, 2, 2,
    2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2,
    2, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2,
    2, 2, 4, 2, 2, 0, 0, 3, 2, 2, 4, 2, 2, 0, 0, 3, 2, 2, 4, 2, 2, 0, 0, 3,
    2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0,
    2, 0, 1, 0, 0, 1, 2, 2, 2, 0, 0, 0, 0, 1, 2, 2, 2, 0, 0, 0, 0, 1, 2, 2,
    2, 0, 1, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};


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


internal void draw_buffer(game_offscreen_buffer_t *buffer, i32 X, i32 Y, game_offscreen_buffer_t *buffer_to_draw) {
    i32 min_x = X;
    i32 min_y = Y;
    i32 max_x = min_x + buffer_to_draw->width;
    i32 max_y = min_y + buffer_to_draw->height;

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

    u8 *row = (((u8 *) buffer->memory) + min_x * buffer->bytes_per_pixel + min_y * buffer->pitch);
    u8 *source_row = buffer_to_draw->memory;

    for (int y = min_y; y < max_y; y++) {
        u32 *pixel = (u32 *) row;
        u32 *source_pixel = (u32 *) source_row;

        for (int x = min_x; x < max_x; x++) {
            *pixel = *source_pixel;
            ++pixel;
            ++source_pixel;
        }

        row += buffer->pitch;
        source_row += buffer_to_draw->pitch;
    }
}


internal void draw_minimap(game_state_t *game, game_offscreen_buffer_t *buffer) {

    i32 x_offset, y_offset;
    f32 r, g, b;

    i32 pixels_per_tile = MINIMAP_PIXELS_PER_TILE;
    i32 map_start_x;
    i32 map_start_y;
    i32 map_end_x;
    i32 map_end_y;

    map_start_x = 0;
    map_start_y = 0;
    map_end_x = MAP_W;
    map_end_y = MAP_H;

    // TODO(max): loop only on a subset of the map
    // u32 how_many_columns = MINIMAP_W / MINIMAP_PIXELS_PER_TILE;
    // u32 how_many_rows = MINIMAP_H / MINIMAP_PIXELS_PER_TILE;

    // i32 map_start_x = (g_player_x / pixels_per_tile) - how_many_columns;
    // i32 map_start_y = (g_player_y / pixels_per_tile) - how_many_rows;

    // if (map_start_x < 0) {
    //     map_start_x = 0;
    // }

    // if (map_start_y < 0) {
    //     map_start_y = 0;
    // }

    // i32 map_end_x = map_start_x + how_many_columns;
    // i32 map_end_y = map_start_y + how_many_rows;

    // if (map_end_x > MAP_W) {
    //     map_end_x = MAP_W;
    // }

    // if (map_end_y > MAP_H) {
    //     map_end_y = MAP_H;
    // }

    f32 screen_center_x = 0.5f * (f32)g_minimap.width;
    f32 screen_center_y = 0.5f * (f32)g_minimap.height;

    // Clear g_minimap
    rect_t full_minimap = {0, 0, g_minimap.width, g_minimap.height};
    draw_rectangle(&g_minimap, &full_minimap, .0f, 1.0f, .0f);

    for (i32 y = map_start_y; y < map_end_y; ++y) {
        for (i32 x = map_start_x; x < map_end_x; ++x) {

            if (map_walls[y * MAP_W + x] > 0) {
                r = 1.0f;
                g = 1.0f;
                b = 1.0f;
            } else {
                r = .0f;
                g = .0f;
                b = .0f;
            }

            x_offset = x * pixels_per_tile - g_player_x;
            y_offset = y * pixels_per_tile - g_player_y;

            rect_t rect = {x_offset, y_offset, pixels_per_tile, pixels_per_tile};
            draw_rectangle(&g_minimap, &rect, r, g, b);
        }
    }

    // Draw player
    f32 player_size = .75f * pixels_per_tile;
    f32 player_half = player_size / 2;
    rect_t rec = {screen_center_x - player_half, screen_center_y - player_half, player_size, player_size};
    draw_rectangle(&g_minimap, &rec, 1.0f, .0f, .0f);

    // Draw g_minimap on screen

    u32 minimap_x = 10;
    u32 minimap_y = buffer->height - g_minimap.height - 10;
    draw_buffer(buffer, minimap_x, minimap_y, &g_minimap);
}


// void game_initialize(game_memory_t *memory, game_offscreen_buffer_t *buffer)
GAME_INITIALIZE(game_initialize) {
    game_state_t *game = (game_state_t *) memory->permanent_storage;
}


// void game_update_and_render(game_memory_t *memory, game_input_t *input, game_offscreen_buffer_t *buffer)
GAME_UPDATE_AND_RENDER(game_update_and_render) {
    game_state_t *game = (game_state_t *) memory->permanent_storage;
    game_controller_input_t *controller = &input->controllers[0];

    if (controller->up.ended_down) {
        g_player_y -= 1;
    }

    if (controller->down.ended_down) {
        g_player_y += 1;
    }

    if (controller->left.ended_down) {
        g_player_x -= 1;
    }

    if (controller->right.ended_down) {
        g_player_x += 1;
    }

    rect_t rec = {.0f, .0f, buffer->width, buffer->height};
    draw_rectangle(buffer, &rec, 1.0f, .0f, 1.0f);
    draw_minimap(game, buffer);
}



/**************************************************************************************************
 *                                             SOUND
 *************************************************************************************************/
void game_output_sound(game_state_t *game_state, game_sound_output_buffer_t *buffer, int tone_Hz) {
    // i16 toneVolume = 3000;
    // f32 wavePeriod = (f32)buffer->samples_per_second / tone_Hz;

    i16 *sample_out = buffer->samples;

    for (int i = 0; i < buffer->sample_count; i++) {
        i16 sample_value = 0;
        *sample_out++ = sample_value;
        *sample_out++ = sample_value;
    }
}


// void game_get_sound_samples(game_memory_t *memory, game_sound_output_buffer_t *sound_buffer)
GAME_GET_SOUND_SAMPLES(game_get_sound_samples) {
    game_state_t *game_state = (game_state_t *) memory->permanent_storage;
    game_output_sound(game_state, sound_buffer, 400);
}