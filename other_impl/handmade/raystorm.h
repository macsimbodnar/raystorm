#pragma once
#include "common_platform.h"

// void game_initialize(game_memory_t *memory, game_offscreen_buffer_t *buffer)
GAME_INITIALIZE(game_initialize);

// void game_update_and_render(game_memory_t *memory, game_input_t *input,
// game_offscreen_buffer_t *buffer)
GAME_UPDATE_AND_RENDER(game_update_and_render);

// void game_get_sound_samples(game_memory_t *memory, game_sound_output_buffer_t
// *sound_buffer)
GAME_GET_SOUND_SAMPLES(game_get_sound_samples);

// void game_close(game_memory_t* memory)
GAME_CLOSE(game_close);
