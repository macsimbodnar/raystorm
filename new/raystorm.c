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


global_var u32 g_m[17 * 9] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};


void gen_world(world_t *world) {
    world->tile_map->tiles = g_m;
    world->tile_map->width = 17;
    world->tile_map->height = 9;
}


internal void draw_rectangle(game_offscreen_buffer_t *buffer, const rect_t rect, const color_t color) {
    u32 min_x = rect.x;
    u32 min_y = rect.y;
    u32 max_x = rect.x + rect.width;
    u32 max_y = rect.y + rect.height;

    if (max_x > buffer->width) {
        max_x = buffer->width;
    }

    if (max_y > buffer->height) {
        max_y = buffer->height;
    }

    if (min_x > max_x) {
        min_x = 0;
    }

    if (min_y > max_y) {
        min_y = 0;
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


internal void draw_buffer(game_offscreen_buffer_t *dest_buffer, const u32 X, const u32 Y, const game_offscreen_buffer_t *source_buffer) {
    u32 min_x = X;
    u32 min_y = Y;
    u32 max_x = min_x + source_buffer->width;
    u32 max_y = min_y + source_buffer->height;

    if (max_x > dest_buffer->width) {
        max_x = dest_buffer->width;
    }

    if (max_y > dest_buffer->height) {
        max_y = dest_buffer->height;
    }

    if (min_x > max_x) {
        min_x = 0;
    }

    if (min_y > max_y) {
        min_y = 0;
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


internal void draw_map(game_state_t *game, game_offscreen_buffer_t *buffer) {
    rect_t tile = {};
    color_t color = {};
    const tile_map_t *map = game->world->tile_map;
    const u32 tile_side_in_pixels = map->tile_side_in_pixels;
    tile.height = tile_side_in_pixels - 2;
    tile.width = tile_side_in_pixels - 2;

    for (u32 y = 0; y < map->height; ++y) {
        for (u32 x = 0; x < map->width; ++x) {
            u8 val = get_tile_value(map, x, y);

            if (val) {
                color = (color_t) {0.8f, 0.8f, 0.8f};
            } else {
                color = (color_t) {0.1f, 0.1f, 0.1f};
            }

            tile.x = x * tile_side_in_pixels + 1;
            tile.y = y * tile_side_in_pixels + 1;

            draw_rectangle(buffer, tile, color);
        }
    }

    real_pos_t player_pos = cart_to_real_pos(map, game->player_pos);

    // Draw player current tile
    rect_t player_tile = {player_pos.tile.X * (map->tile_side_in_pixels), player_pos.tile.Y * (map->tile_side_in_pixels), map->tile_side_in_pixels, map->tile_side_in_pixels};
    draw_rectangle(buffer, player_tile, GREEN);

    // Draw player
    f32 meters_to_pixels = (f32)(map->tile_side_in_pixels / map->tile_side_in_meters);
    rect_t player_rect = (rect_t) {
        (player_pos.tile.X * (map->tile_side_in_pixels)) + round_f32_to_u32(player_pos.offset.X * meters_to_pixels),
        (player_pos.tile.Y * (map->tile_side_in_pixels)) + round_f32_to_u32(player_pos.offset.Y * meters_to_pixels) - game->player_size,
        game->player_size,
        game->player_size
    };
    draw_rectangle(buffer, player_rect, RED);
}


internal void update_player(const game_input_t *input, game_state_t *game) {
    const game_controller_input_t *controller = &input->controllers[0];

    if (controller->up.ended_down) {
        game->player_pos.offset.Y += 0.1f;
    }

    if (controller->down.ended_down) {
        game->player_pos.offset.Y -= 0.1f;
    }

    if (controller->left.ended_down) {
        game->player_pos.offset.X -= 0.1f;
    }

    if (controller->right.ended_down) {
        game->player_pos.offset.X += 0.1f;
    }

    recanonicalize_pos(game->world->tile_map, &game->player_pos);
}


// void game_initialize(game_memory_t *memory, game_offscreen_buffer_t *buffer)
GAME_INITIALIZE(game_initialize) {
    // game_state_t *game = (game_state_t *) memory->permanent_storage;
    game_state_t *game = &g_game;

    // Init player pos
    game->player_size = 40;
    game->player_pos.tile = (point_u32_t) {0, 0};
    game->player_pos.offset = (point_f32_t) {0.0f, 0.0f};
    game->player_velocity = (v2_t) {.0f, .0f};

    // Init camera pos
    game->camera_pos.tile = (point_u32_t) {17 / 2, 9 / 2};
    game->camera_pos.offset = (point_f32_t) {0.f, .0f};

    // Set world
    // TODO(max): Do we need the world_t struct or should we just use the tile_map_t?
    game->world = &g_world;
    game->world->tile_map = &g_tile_map;

    // Set tilemap
    game->world->tile_map->tile_side_in_meters = 1.4f;
    game->world->tile_map->tile_side_in_pixels = 58;

    gen_world(game->world);
}


// void game_update_and_render(game_memory_t *memory, game_input_t *input, game_offscreen_buffer_t *buffer)
GAME_UPDATE_AND_RENDER(game_update_and_render) {
    // game_state_t *game = (game_state_t *) memory->permanent_storage;
    game_state_t *game = &g_game;

    // Clear screen
    rect_t rec = {.0f, .0f, buffer->width, buffer->height};
    draw_rectangle(buffer, rec, BLACK);

    // Update the player pos
    update_player(input, game);

    draw_map(game, buffer);
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