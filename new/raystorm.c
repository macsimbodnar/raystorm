#include "raystorm.h"
#include "intrinsics.h"
#include "trigonometry.h"
#include "random.h"
#include "log.h"


global_var color_t RED =        {1.0f, .0f, .0f};
global_var color_t GREEN =      {.0f, 1.0f, .0f};
global_var color_t BLUE =       {.0f, .0f, 1.0f};
global_var color_t WHITE =      {1.0f, 1.0f, 1.0f};
global_var color_t BLACK =      {.0f, .0f, .0f};
global_var color_t PURPLE =     {1.0f, .0f, 1.0f};

global_var game_state_t g_game;
global_var world_t g_world;
global_var tile_map_t g_tile_map;
global_var tile_chunk_t g_chunks;


global_var u32 g_m[17 * 9] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
};

void gen_world(world_t *world) {
    world->tile_map->chunks->tiles = g_m;
}


internal void draw_rectangle(game_offscreen_buffer_t *buffer, const rect_t rect, const color_t color) {
    i32 min_x = rect.x;
    i32 min_y = rect.y;
    i32 max_x = rect.x + rect.width;
    i32 max_y = rect.y + rect.height;

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


internal void draw_buffer(game_offscreen_buffer_t *dest_buffer, const i32 X, const i32 Y, const game_offscreen_buffer_t *source_buffer) {
    i32 min_x = X;
    i32 min_y = Y;
    i32 max_x = min_x + source_buffer->width;
    i32 max_y = min_y + source_buffer->height;

    if (min_x < 0) {
        min_x = 0;
    }

    if (min_y < 0) {
        min_y = 0;
    }

    if (max_x > dest_buffer->width) {
        max_x = dest_buffer->width;
    }

    if (max_y > dest_buffer->height) {
        max_y = dest_buffer->height;
    }

    u8 *row = (((u8 *) dest_buffer->memory) + min_x * dest_buffer->bytes_per_pixel + min_y * dest_buffer->pitch);
    u8 *source_row = source_buffer->memory;

    for (int y = min_y; y < max_y; y++) {
        u32 *pixel = (u32 *) row;
        u32 *source_pixel = (u32 *) source_row;

        for (int x = min_x; x < max_x; x++) {
            *pixel = *source_pixel;
            ++pixel;
            ++source_pixel;
        }

        row += dest_buffer->pitch;
        source_row += source_buffer->pitch;
    }
}


internal void update_player(const game_input_t *input) {
    const game_controller_input_t *controller = &input->controllers[0];

    if (controller->up.ended_down) {

    }

    if (controller->down.ended_down) {

    }

    if (controller->left.ended_down) {

    }

    if (controller->right.ended_down) {

    }
}


// void game_initialize(game_memory_t *memory, game_offscreen_buffer_t *buffer)
GAME_INITIALIZE(game_initialize) {
    // game_state_t *game = (game_state_t *) memory->permanent_storage;
    game_state_t *game = &g_game;

    // Init player pos
    game->player_pos.abs_tile = (point_u32_t) {2, 3};
    game->player_pos.offset = (point_f32_t) {1.0f, 1.0f};
    game->player_velocity = (v2_t) {.0f, .0f};

    // Init camera pos
    game->camera_pos.abs_tile = (point_u32_t) {17 / 2, 9 / 2};
    game->camera_pos.offset = (point_f32_t) {0.f, .0f};

    // Set world
    // TODO(max): Do we need the world_t struct or should we just use the tile_map_t?
    game->world = &g_world;
    game->world->tile_map = &g_tile_map;

    // Set tilemap
    game->world->tile_map->chunks_count_x = 1;
    game->world->tile_map->chunks_count_y = 1;
    game->world->tile_map->tile_side_in_meters = 1.4f;

    game->world->tile_map->chunks = &g_chunks;
    gen_world(game->world);
}


internal void draw_map(game_offscreen_buffer_t *buffer, game_state_t *game) {

    u32 tile_side_in_pixels = 60;
    f32 meters_to_pixels = (f32)(tile_side_in_pixels / game->world->tile_map->tile_side_in_meters);
    point_f32_t screen_center = (point_f32_t) {0.5f * (f32)buffer->width, 0.5f * (f32)buffer->height};

    for (i32 rel_row = -100; rel_row < 100; ++rel_row) {
        for (i32 rel_column = -200; rel_column < 200; ++rel_column) {

            u32 column = game->camera_pos.abs_tile.X + rel_column;
            u32 row = game->camera_pos.abs_tile.Y + rel_row;

            abs_tile_pos_t pos;
            pos.tile_chunk.X = column;
            pos.tile_chunk.Y = row;
            pos.tile_index_in_chunk = (point_u32_t) {0, 0};
            u32 tile_id = get_tile_value(game->world->tile_map, pos);

            if (tile_id > 1) {
                f32 gray = 0.4f;

                if (tile_id == 2) {
                    gray = 0.8f;
                }

                if (tile_id > 2) {
                    gray = 0.25f;
                }

                if ((column == game->camera_pos.abs_tile.X) && (row == game->camera_pos.abs_tile.Y)) {
                    gray = 0.0f;
                }

                point_f32_t tile_side = {0.5f * tile_side_in_pixels, 0.5f * tile_side_in_pixels};

                point_f32_t cen = (point_f32_t) {
                    screen_center.X - meters_to_pixels *game->camera_pos.offset.X + ((f32) rel_column) *tile_side_in_pixels,
                                    screen_center.Y + meters_to_pixels *game->camera_pos.offset.Y - ((f32) rel_row) *tile_side_in_pixels
                };

                point_f32_t min = (point_f32_t) {cen.X - tile_side.X, cen.Y - tile_side.Y};
                point_f32_t max = (point_f32_t) {cen.X + tile_side.X, cen.Y + tile_side.Y};

                drawRectangle(buffer, min, max, gray, gray, gray);
            }
        }
    }
}



// void game_update_and_render(game_memory_t *memory, game_input_t *input, game_offscreen_buffer_t *buffer)
GAME_UPDATE_AND_RENDER(game_update_and_render) {
    // game_state_t *game = (game_state_t *) memory->permanent_storage;
    game_state_t *game = &g_game;

    // Clear screen
    rect_t rec = {.0f, .0f, buffer->width, buffer->height};
    draw_rectangle(buffer, rec, PURPLE);

    // Update the player pos
    update_player(input);

    draw_map(buffer, game);
}



/**************************************************************************************************
 *                                             SOUND
 *************************************************************************************************/
void game_output_sound(game_state_t *game_state, game_sound_output_buffer_t *buffer, const int tone_Hz) {
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