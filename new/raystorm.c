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


global_var u32 g_m[17 * 9] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
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
    rect_t tile = {};
    color_t color = {};
    const tile_map_t *map = game->world->tile_map;
    const u32 tile_side_in_pixels = map->tile_side_in_pixels;
    tile.height = tile_side_in_pixels - 2;
    tile.width = tile_side_in_pixels - 2;

    for (u32 y = 0; y < map->height; ++y) {
        for (u32 x = 0; x < map->width; ++x) {
            u8 val = get_tile_value_in_raw_coordinates(map, x, y);

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
}


internal void draw_minimap(game_state_t *game, game_offscreen_buffer_t *buffer) {
    rect_t tile = {};
    color_t color = {};
    const tile_map_t *map = game->world->tile_map;
    const u32 tile_side_in_pixels = map->tile_side_in_pixels / 4;
    tile.height = tile_side_in_pixels - 2;
    tile.width = tile_side_in_pixels - 2;

    for (u32 y = 0; y < map->height; ++y) {
        for (u32 x = 0; x < map->width; ++x) {
            u8 val = get_tile_value_in_raw_coordinates(map, x, y);

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
}


internal void draw_player_on_minimap(game_state_t *game, game_offscreen_buffer_t *buffer) {
    const tile_map_t *map = game->world->tile_map;

    real_pos_t player_pos = cart_to_real_pos(map, game->player_pos);

    // Draw player current tile
    rect_t player_tile = {player_pos.tile.X *(map->tile_side_in_pixels), player_pos.tile.Y *(map->tile_side_in_pixels), map->tile_side_in_pixels / 4, map->tile_side_in_pixels / 4};
    draw_rectangle(buffer, player_tile, YELLOW);

    // Draw player
    f32 meters_to_pixels = (f32)(map->tile_side_in_pixels / map->tile_side_in_meters);
    rect_t player_rect = (rect_t) {
        (player_pos.tile.X * (map->tile_side_in_pixels)) + round_f32_to_i32(player_pos.offset.X * meters_to_pixels) - game->player_size / 2,
        (player_pos.tile.Y * (map->tile_side_in_pixels)) + round_f32_to_i32(player_pos.offset.Y * meters_to_pixels) - game->player_size / 2,
        game->player_size / 4,
        game->player_size / 4
    };
    draw_rectangle(buffer, player_rect, RED);


    f32 pdx = cos_f32(game->player_angle);
    f32 pdy = -sin_f32(game->player_angle);

    point_i32_t start_point = {player_rect.x + game->player_size / 2, player_rect.y + game->player_size / 2};
    point_i32_t end_point;
    end_point.X = start_point.X + round_f32_to_i32(pdx * 30) / 4;
    end_point.Y = start_point.Y + round_f32_to_i32(pdy * 30) / 4;

    draw_line(buffer, start_point, end_point, GREEN);
}


internal void draw_player(game_state_t *game, game_offscreen_buffer_t *buffer) {
    const tile_map_t *map = game->world->tile_map;

    real_pos_t player_pos = cart_to_real_pos(map, game->player_pos);

    // Draw player current tile
    rect_t player_tile = {player_pos.tile.X *(map->tile_side_in_pixels), player_pos.tile.Y *(map->tile_side_in_pixels), map->tile_side_in_pixels, map->tile_side_in_pixels};
    draw_rectangle(buffer, player_tile, YELLOW);

    // Draw player
    f32 meters_to_pixels = (f32)(map->tile_side_in_pixels / map->tile_side_in_meters);
    rect_t player_rect = (rect_t) {
        (player_pos.tile.X * (map->tile_side_in_pixels)) + round_f32_to_i32(player_pos.offset.X * meters_to_pixels) - game->player_size / 2,
        (player_pos.tile.Y * (map->tile_side_in_pixels)) + round_f32_to_i32(player_pos.offset.Y * meters_to_pixels) - game->player_size / 2,
        game->player_size,
        game->player_size
    };
    draw_rectangle(buffer, player_rect, RED);


    // for (int i = 0; i < 17; i++) {
    //     draw_line(buffer, (point_i32_t) {player_rect.x + game->player_size / 2, player_rect.y + game->player_size / 2}, (point_i32_t) {58 * i + (58 / 2), 0}, GREEN);
    // }

    // for (int i = 0; i < 17; i++) {
    //     draw_line(buffer, (point_i32_t) {player_rect.x + game->player_size / 2, player_rect.y + game->player_size / 2}, (point_i32_t) {58 * i + (58 / 2), 58 * 9}, GREEN);
    // }

    // for (int i = 0; i < 9; i++) {
    //     draw_line(buffer, (point_i32_t) {player_rect.x + game->player_size / 2, player_rect.y + game->player_size / 2}, (point_i32_t) {0, 58 * i + (58 / 2)}, GREEN);
    // }

    // for (int i = 0; i < 9; i++) {
    //     draw_line(buffer, (point_i32_t) {player_rect.x + game->player_size / 2, player_rect.y + game->player_size / 2}, (point_i32_t) {58 * 17, 58 * i + (58 / 2)}, GREEN);
    // }

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

    v2_t player_second_derivate = {};
    f32 direction = .0f;

    if (controller->up.ended_down) {
        // player_second_derivate.Y = 1.0f;
        direction = 1.0f;
    }

    if (controller->down.ended_down) {
        // player_second_derivate.Y = -1.0f;
        direction = - 1.0f;
    }

    if (controller->left.ended_down) {
        // player_second_derivate.X = -1.0f;

        game->player_angle += 0.03f;

        if (game->player_angle < 0) {
            game->player_angle += 2 * PI;
        }
    }

    if (controller->right.ended_down) {
        // player_second_derivate.X = 1.0f;

        game->player_angle -= 0.03f;

        if (game->player_angle > 2 * PI) {
            game->player_angle -= 2 * PI;
        }
    }

    player_second_derivate.X = cos_f32(game->player_angle);
    player_second_derivate.Y = sin_f32(game->player_angle);

    // If we are traveling not on x ot y axis
    if (player_second_derivate.X != 0.0f && player_second_derivate.Y != 0.0f) {
        // Normalizing the length so we don't move faster on the diagonal
        player_second_derivate = scalar_mul_f32(player_second_derivate, 0.707106781187f);
    }

    const f32 speed = 45.0f * direction;                     // [m/s^2]
    player_second_derivate = scalar_mul_f32(player_second_derivate, speed);

    // Deceleration
    player_second_derivate = vec_sum_f32(player_second_derivate, scalar_mul_f32(game->player_velocity, -3.5f));

    // Calculate the new player position based on his velocity and the time elapsed from the last check
    world_pos_t new_player_pos = game->player_pos;
    v2_t A = scalar_mul_f32(scalar_mul_f32(player_second_derivate, 1.0f), squaref(input->dt_for_frame));
    v2_t B = scalar_mul_f32(game->player_velocity, input->dt_for_frame);
    new_player_pos.offset =  vec_sum_f32(new_player_pos.offset, vec_sum_f32(A, B));
    game->player_velocity = vec_sum_f32(game->player_velocity, scalar_mul_f32(player_second_derivate, input->dt_for_frame));

    recanonicalize_pos(game->world->tile_map, &new_player_pos);

    // Check for wall collision
    bool32 collided = !is_tile_empty(game->world->tile_map, new_player_pos.tile.X, new_player_pos.tile.Y);

    if (collided) {
        v2_t r = {};

        if (new_player_pos.tile.X < game->player_pos.tile.X) {
            r = (v2_t) {1, 0};
        }

        if (new_player_pos.tile.X > game->player_pos.tile.X) {
            r = (v2_t) {-1, 0};
        }

        if (new_player_pos.tile.Y < game->player_pos.tile.Y) {
            r = (v2_t) {0, 1};
        }

        if (new_player_pos.tile.Y > game->player_pos.tile.Y) {
            r = (v2_t) {0, -1};
        }

        // NOTE(max): this on make the hero bounce
        // gameState->dPlayerP = gameState->dPlayerP - 2.0f * inner(gameState->dPlayerP, r) * r;

        // this one make it slide on the wall
        // game->player_velocity = vec_sum_f32(game->player_velocity, scalar_mul_f32(r, (- 1.0f * inner_mul_f32(game->player_velocity, r))));
    } else {
        game->player_pos = new_player_pos;
    }
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
    game->world->tile_map->tile_side_in_pixels = 58;

    gen_world(game->world);

    // Init player pos
    game->player_size = 40;
    game->player_angle = 1.5708f;   // 90 degrees in radiants
    game->player_pos.tile = (point_i32_t) {1, 1};
    game->player_pos.offset = (point_f32_t) {game->world->tile_map->tile_side_in_meters / 2, game->world->tile_map->tile_side_in_meters / 2};
    game->player_velocity = (v2_t) {.0f, .0f};

    // Init camera pos
    game->camera_pos.tile = (point_i32_t) {17 / 2, 9 / 2};
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

    // Clear screen
    rect_t rec = {.0f, .0f, buffer->width, buffer->height};
    draw_rectangle(buffer, rec, PURPLE);

    // Update the player pos
    update_player(input, game);

    draw_map(game, buffer);
    draw_player(game, buffer);

    draw_minimap(game, &g_minimap);
    // draw_player_on_minimap(game, &g_minimap);

    draw_buffer(buffer, 0, 0, &g_minimap);
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