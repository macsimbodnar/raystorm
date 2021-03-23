#pragma once
#include "common_platform.h"


typedef struct {
    // memory_arena worldArena;
    // world *_world;

    // tile_map_position cameraP;
    // tile_map_position playerP;
    // v2 dPlayerP;                 // first derivative (player velocity)

    // loaded_bitmap backdrop;
    // uint32 heroFacingDirection;
    // hero_bitmaps heroBitmaps[4];
} game_state_t;


// void game_update_and_render(game_memory_t *memory, game_input_t *input, game_offscreen_buffer_t *buffer)
GAME_UPDATE_AND_RENDER(game_update_and_render);


// void game_get_sound_samples(game_memory_t *memory, game_sound_output_buffer_t *sound_buffer)
GAME_GET_SOUND_SAMPLES(game_get_sound_samples);