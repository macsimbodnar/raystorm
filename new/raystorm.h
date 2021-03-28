#pragma once
#include "common_platform.h"
#include "tile.h"


typedef struct {
    point_u32_t abs_tile;
    point_f32_t offset;
} world_pos_t;


typedef struct {
    tile_map_t *tile_map;
} world_t;


typedef struct {
    world_t            *world;

    world_pos_t         camera_pos;
    world_pos_t         player_pos;
    v2_t                player_velocity;               // first derivate
} game_state_t;


// void game_initialize(game_memory_t *memory, game_offscreen_buffer_t *buffer)
GAME_INITIALIZE(game_initialize);


// void game_update_and_render(game_memory_t *memory, game_input_t *input, game_offscreen_buffer_t *buffer)
GAME_UPDATE_AND_RENDER(game_update_and_render);


// void game_get_sound_samples(game_memory_t *memory, game_sound_output_buffer_t *sound_buffer)
GAME_GET_SOUND_SAMPLES(game_get_sound_samples);