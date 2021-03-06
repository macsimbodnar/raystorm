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
global_var color_t YELLOW =     {1.0f, 1.0f, .0f};

global_var game_state_t g_game;
global_var world_t g_world;
global_var tile_map_t g_tile_map;
global_var u8 g_minimap_memory[500 * 500 * 4];
global_var game_offscreen_buffer_t g_minimap;


// global_var u32 g_m[17 * 9] = {
//     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
// };

#define MAP_WIDTH   24
#define MAP_HEIGHT  24

global_var u32 g_m[MAP_WIDTH * MAP_HEIGHT] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 4, 0, 0, 0, 0, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 4, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};


void gen_world(world_t *world) {
    world->tile_map->tiles = g_m;
    world->tile_map->width = MAP_WIDTH;
    world->tile_map->height = MAP_HEIGHT;
}


internal void draw_line(game_offscreen_buffer_t *buffer, point_i32_t A, point_i32_t B, const color_t color) {
    // Use Bresenham's principles of integer incremental error to perform all octant line draws

    // *INDENT-OFF*
    if (A.X > buffer->width)    A.X = buffer->width;
    if (A.X < 0)                A.X = 0;
    if (A.Y > buffer->height)   A.Y = buffer->height;
    if (A.Y < 0)                A.Y = 0;
    if (B.X > buffer->width)    B.X = buffer->width;
    if (B.X < 0)                B.X = 0;
    if (B.Y > buffer->height)   B.Y = buffer->height;
    if (B.Y < 0)                B.Y = 0;
    // *INDENT-ON*

    u32 *pixel;
    u32 color_32 = ((round_f32_to_u32(color.R * 255.0f) << 16) | (round_f32_to_u32(color.G * 255.0f) << 8) | (round_f32_to_u32(color.B * 255.0f) << 0));

    i32 dx =  abs_i32(A.X - B.X);
    i32 sx = B.X < A.X ? 1 : -1;
    i32 dy = -abs_i32(A.Y - B.Y);
    i32 sy = B.Y < A.Y ? 1 : -1;
    i32 err = dx + dy;
    i32 x = B.X;
    i32 y = B.Y;
    i32 e2;

    while (true) {

        pixel = (u32 *)(((u8 *) buffer->memory) + x * buffer->bytes_per_pixel + y * buffer->pitch);
        *pixel = color_32;

        if (x == A.X && y == A.Y) {
            break;
        }

        e2 = 2 * err;

        if (e2 >= dy) {
            err += dy;
            x += sx;
        }

        if (e2 <= dx) {
            err += dx;
            y += sy;
        }
    }
}


internal void draw_rectangle(game_offscreen_buffer_t *buffer, const rect_t rect, const color_t color) {
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


internal void draw_buffer(game_offscreen_buffer_t *dest_buffer, const i32 X, const i32 Y, const game_offscreen_buffer_t *source_buffer) {
    i32 min_x = X;
    i32 min_y = Y;
    i32 max_x = min_x + source_buffer->width;
    i32 max_y = min_y + source_buffer->height;

    // *INDENT-OFF*
    if (min_x < 0)                      min_x = 0;
    if (min_y < 0)                      min_y = 0;
    if (max_x > dest_buffer->width)     max_x = dest_buffer->width;
    if (max_y > dest_buffer->height)    max_y = dest_buffer->height;
    // *INDENT-ON*

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

    const tile_map_t *map = game->world->tile_map;
    const real_pos_t camera = cart_to_real_pos(map, game->camera_pos);
    const f32 meters_to_pixels = (f32)(map->tile_side_in_pixels / map->tile_side_in_meters);
    const i32 camera_offset_x = round_f32_to_i32(camera.offset.X * meters_to_pixels);
    const i32 camera_offset_y = round_f32_to_i32(camera.offset.Y * meters_to_pixels);
    const i32 half_buffer_w = buffer->width / 2;
    const i32 half_buffer_h = buffer->height / 2;

    const u32 tile_side_in_pixels = map->tile_side_in_pixels;

    color_t color = {};
    rect_t tile = {};
    tile.height = tile_side_in_pixels - 2;
    tile.width = tile_side_in_pixels - 2;

    // TODO(max): Loop only on the in screen tails
    i32 min_x = 0;
    i32 min_y = 0;
    i32 max_x = map->width;
    i32 max_y = map->height;

    for (i32 y = min_y; y < max_y; ++y) {
        for (i32 x = min_x; x < max_x; ++x) {
            u8 val = get_tile_value_in_raw_coordinates(map, x, y);

            if (val) {
                color = (color_t) {0.8f, 0.8f, 0.8f};
            } else {
                color = (color_t) {0.1f, 0.1f, 0.1f};
            }

            tile.x = 1 + (half_buffer_w + (x - camera.tile.X) * tile_side_in_pixels) - camera_offset_x;
            tile.y = 1 + (half_buffer_h + (y - camera.tile.Y) * tile_side_in_pixels) - camera_offset_y;

            // tile.x = x * tile_side_in_pixels + 1;
            // tile.y = y * tile_side_in_pixels + 1;

            draw_rectangle(buffer, tile, color);
        }
    }
}


internal void draw_player(game_state_t *game, game_offscreen_buffer_t *buffer) {

    const tile_map_t *map = game->world->tile_map;
    const real_pos_t player_pos = cart_to_real_pos(map, game->player_pos);
    const f32 meters_to_pixels = (f32)(map->tile_side_in_pixels / map->tile_side_in_meters);

    // Player rect
    i32 half_player = game->player_size / 2;
    i32 x = (buffer->width / 2) - half_player;
    i32 y = (buffer->height / 2) - half_player;
    rect_t player_rect = {x, y, game->player_size, game->player_size};

    // player current tile rect
    rect_t player_tile = {
        (buffer->width / 2) - round_f32_to_i32(player_pos.offset.X * meters_to_pixels),
        (buffer->height / 2) - round_f32_to_i32(player_pos.offset.Y * meters_to_pixels),
        map->tile_side_in_pixels,
        map->tile_side_in_pixels
    };

    // Draw player current tile
    draw_rectangle(buffer, player_tile, YELLOW);
    // Draw player pos
    draw_rectangle(buffer, player_rect, RED);

    // Draw the player direction ray
    f32 pdx = cos_f32(game->player_angle);
    f32 pdy = -sin_f32(game->player_angle);

    point_i32_t start_point = {player_rect.x + game->player_size / 2, player_rect.y + game->player_size / 2};
    point_i32_t end_point;
    end_point.X = start_point.X + round_f32_to_i32(pdx * 30);
    end_point.Y = start_point.Y + round_f32_to_i32(pdy * 30);

    draw_line(buffer, start_point, end_point, GREEN);
}


internal void update_player(const game_input_t *input, game_state_t *game) {
    const game_controller_input_t *controller = &input->controllers[0];

    v2_t player_offset = {};
    f32 direction = .0f;

    // TODO(max): Update the angle based on the elapsed time and not by a const amount
    if (controller->up.ended_down) {
        direction += 1.0f;
    }

    if (controller->down.ended_down) {
        direction += - 1.0f;
    }

    if (controller->left.ended_down) {
        game->player_angle += 0.03f;

        if (game->player_angle < 0) {
            game->player_angle += 2 * PI;
        }
    }

    if (controller->right.ended_down) {
        game->player_angle -= 0.03f;

        if (game->player_angle > 2 * PI) {
            game->player_angle -= 2 * PI;
        }
    }

    const f32 delta_space = 0.005f * direction / input->dt_for_frame;

    player_offset.X = cos_f32(game->player_angle) * delta_space;
    player_offset.Y = sin_f32(game->player_angle) * delta_space;

    // Calculate the new player position based on his velocity and the time elapsed from the last check
    world_pos_t new_player_pos = game->player_pos;
    new_player_pos.offset = vec_sum_f32(new_player_pos.offset, player_offset);

    recanonicalize_pos(game->world->tile_map, &new_player_pos);

    game->player_pos = new_player_pos;
    game->camera_pos = game->player_pos;
}


// void game_initialize(game_memory_t *memory, game_offscreen_buffer_t *buffer)
GAME_INITIALIZE(game_initialize) {
    // game_state_t *game = (game_state_t *) memory->permanent_storage;
    game_state_t *game = &g_game;

    // Set world
    // TODO(max): Do we need the world_t struct or should we just use the tile_map_t?
    game->world = &g_world;
    game->world->tile_map = &g_tile_map;

    // Set tilemap
    game->world->tile_map->tile_side_in_meters = 1.4f;
    game->world->tile_map->tile_side_in_pixels = 20;

    gen_world(game->world);

    // Init player pos
    game->player_size = round_f32_to_i32(game->world->tile_map->tile_side_in_pixels * 0.6f);
    game->player_angle = 1.5708f;   // 90 degrees in radiants
    game->player_pos.tile = (point_i32_t) {1, 1};
    game->player_pos.offset = (point_f32_t) {game->world->tile_map->tile_side_in_meters / 2, game->world->tile_map->tile_side_in_meters / 2};

    // Init camera pos
    game->camera_pos.tile = (point_i32_t) {game->world->tile_map->width / 2, game->world->tile_map->height / 2};
    game->camera_pos.offset = (point_f32_t) {0.f, .0f};

    g_minimap.bytes_per_pixel = 4;
    g_minimap.height = buffer->height / 4;
    g_minimap.width = buffer->width / 4;
    g_minimap.pitch = g_minimap.width * g_minimap.bytes_per_pixel;
    g_minimap.memory = (void *) &g_minimap_memory;
}


// void game_update_and_render(game_memory_t *memory, game_input_t *input, game_offscreen_buffer_t *buffer)
GAME_UPDATE_AND_RENDER(game_update_and_render) {
    // game_state_t *game = (game_state_t *) memory->permanent_storage;
    game_state_t *game = &g_game;

    // Update the player pos
    update_player(input, game);

    /**
     * Draw world
     */
    // Clear screen
    const rect_t ceiling = {0, 0, buffer->width, buffer->height / 2};
    const color_t ceiling_color = {0.55f, .95f, .90f};
    draw_rectangle(buffer, ceiling, ceiling_color);
    const rect_t floor = {0, buffer->height / 2, buffer->width, buffer->height / 2};
    const color_t floor_color = {0.75f, .25f, .20f};
    draw_rectangle(buffer, floor, floor_color);

    // draw_map(game, buffer);
    // draw_player(game, buffer);

    /**
     * Draw minimap
     */

    // Clear screen
    rect_t mini_rec = {.0f, .0f, g_minimap.width, g_minimap.height};
    draw_rectangle(&g_minimap, mini_rec, BLACK);

    draw_map(game, &g_minimap);
    draw_player(game, &g_minimap);
    draw_buffer(buffer, 10, buffer->height - g_minimap.height - 10, &g_minimap);
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