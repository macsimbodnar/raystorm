#include "log.h"
#include "raystorm.h"

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
    LOG_D("Update image");
}


// void game_get_sound_samples(game_memory_t *memory, game_sound_output_buffer_t *sound_buffer)
GAME_GET_SOUND_SAMPLES(game_get_sound_samples) {
    LOG_D("Update audio");
    game_state_t *game_state = (game_state_t *) memory->permanent_storage;
    game_output_sound(game_state, sound_buffer, 400);
}