#pragma once
#include <stdint.h>


#if RS_ASSERTS
#define ASSERT(expression) if (!(expression)) { *(int *)0 = 0; }
#else
#define ASSERT(expression)
#endif




#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define Kilobytes(value) ((value) * 1024)
#define Megabytes(value) (Kilobytes(value) * 1024)
#define Gigabytes(value) (Megabytes(value) * 1024)
#define Terabytes(value) (Gigabytes(value) * 1024)

#define Pi32 3.14159265359f

#define internal static
#define local_persist static
#define global_var static

#define true                1
#define false               0



typedef int8_t              i8;
typedef int16_t             i16;
typedef int32_t             i32;
typedef int64_t             i64;

typedef uint8_t             u8;
typedef uint16_t            u16;
typedef uint32_t            u32;
typedef uint64_t            u64;

typedef unsigned long       mem_index;

typedef float               f32;
typedef double              f64;

typedef i32                 bool32;




typedef struct {
    int half_transition_count;
    bool32 ended_down;
} game_button_state_t;


typedef struct {
    bool32 is_analog;
    bool32 is_connected;

    f32 average_lx;
    f32 average_ly;
    f32 average_rx;
    f32 average_ry;
    f32 average_tl;
    f32 average_tr;

    union {
        game_button_state_t buttons[18];

        struct {
            game_button_state_t trigger_l;
            game_button_state_t trigger_r;
            game_button_state_t stick_lx;
            game_button_state_t stick_ly;
            game_button_state_t stick_rx;
            game_button_state_t stick_ry;
            game_button_state_t up;
            game_button_state_t down;
            game_button_state_t left;
            game_button_state_t right;
            game_button_state_t back;
            game_button_state_t start;
            game_button_state_t x;
            game_button_state_t y;
            game_button_state_t a;
            game_button_state_t b;
            game_button_state_t l_b;
            game_button_state_t r_b;
        };
    };

} game_controller_input_t;


typedef struct {
    game_button_state_t mouse_buttons[5];
    i32 mouse_x;
    i32 mouse_y;
    i32 mouse_z;

    f32 dt_for_frame;

    game_controller_input_t controllers[5];

    bool32 executable_reloaded;
} game_input_t;


typedef struct {
    u32 queued_audio_bytes;
    u32 output_byte_count;
    u32 expected_bytes_until_flip;
}  DEBUG_sdl_time_marker_t;


typedef struct {
    // NOTE(max): Pixel are 32-bits wide, Memory order BB GG RR XX
    void *memory;
    int width;
    int height;
    int pitch;
    int bytes_per_pixel;
} game_offscreen_buffer_t;


typedef struct {
    int samples_per_second;
    int sample_count;
    i16 *samples;
} game_sound_output_buffer_t;


typedef struct {
    bool32 is_initialized;
    u64 permanent_storage_size;
    // NOTE(max): required to be cleared to zero at startup
    void *permanent_storage;

    u64 transient_storage_size;
    void *transient_storage;
} game_memory_t;


#define GAME_UPDATE_AND_RENDER(name) void name(game_memory_t *memory, game_input_t *input, game_offscreen_buffer_t *buffer)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render_fn);
// GAME_UPDATE_AND_RENDER(gameUpdateAndRenderStub)
// {
// }


#define GAME_GET_SOUND_SAMPLES(name) void name(game_memory_t *memory, game_sound_output_buffer_t *sound_buffer)
typedef GAME_GET_SOUND_SAMPLES(game_get_sound_samples_fn);
// GAME_GET_SOUND_SAMPLES(gameGetSoundSamplesStub)
// {
// }