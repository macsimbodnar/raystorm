#include <SDL2/SDL.h>
#include <sys/mman.h>               // Used for mmap flags
#include <x86intrin.h>              // used for _rdtsc
#include "log.h"
#include "common_platform.h"
#include "raystorm.h"


// To suppress vs code error
#ifndef MAP_ANON
#define MAP_ANON 0
#endif
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0
#endif


/*****************************************************************************
 *                                  DEFINES
 *****************************************************************************/
#define SDL_WIDTH                   1024
#define SDL_HEIGHT                  540
#define SDL_STATE_FILE_NAME_COUNT   4096
#define MAX_CONTROLLERS 4
#define CONTROLLER_AXIS_LEFT_DEADZONE 7849
#define CONTROLLER_AXIS_RIGHT_DEADZONE 7849
#define CONTROLLER_AXIS_TRIGGER_DEADZONE 7849

/*****************************************************************************
 *                                  MACROS
 *****************************************************************************/
#define CHECK_ERR(__exp__) {\
    if (__exp__ != 0) {\
        LOG_E("Error at %s -> %s:%d", __func__, __FILE__, __LINE__);\
    }\
}

#define CHECK_NULL(__exp__) {\
    if (__exp__ == NULL) {\
        LOG_E("Error at %s -> %s:%d", __func__, __FILE__, __LINE__);\
    }\
}



/*****************************************************************************
 *                             DATASTRUCTURES
 *****************************************************************************/
typedef struct {
    // NOTE(max): Pixels are always 32-bits wide, Memory Order BB GG RR XX
    SDL_Texture *texture;
    void *memory;
    int width;
    int height;
    int pitch;
    int bytes_per_pixel;
} sdl_offscreen_buffer_t;


typedef struct {
    int samples_per_second;
    u32 running_sample_index;
    int bytes_per_sample;
    u32 secondary_buffer_size;
    u32 safety_bytes;

    // TODO(max): Should running sample index be in bytes as well
    // TODO(max): Math gets simpler if we add a "bytes per second" field?
} sdl_sound_output_t;


typedef struct {
    u64 total_size;
    void *game_memory_block;
} sdl_state_t;


typedef struct {
    int width;
    int height;
} sdl_window_dimension_t;


typedef struct {
    game_initialize_fn          *initialize;
    game_update_and_render_fn   *update_and_render;
    game_get_sound_samples_fn   *get_sound_samples;
} sdl_game_code_t;




/*****************************************************************************
 *                              GLOBAL VARIABLES
 *****************************************************************************/
global_var i64                      g_perf_counter_frequency;
global_var bool32                   g_show_cursor;
global_var bool32                   g_running;
global_var bool32                   g_pause;
global_var sdl_offscreen_buffer_t   g_back_buffer;
global_var sdl_sound_output_t       g_sound_output;
global_var i16                      *g_audio_samples;
global_var SDL_GameController       *controller_handles[MAX_CONTROLLERS];
global_var SDL_Haptic               *rumble_handles[MAX_CONTROLLERS];




/*****************************************************************************
 *                             SDL UTIL FUNCTIONS
 *****************************************************************************/
internal void sdl_resize_texture(sdl_offscreen_buffer_t *buffer, SDL_Renderer *renderer, int width, int height) {
    if (buffer->memory) {
        munmap(buffer->memory, (buffer->width * buffer->height * buffer->bytes_per_pixel));
    }

    buffer->width = width;
    buffer->height = height;

    int bytesPerPixel = 4;
    buffer->bytes_per_pixel = bytesPerPixel;

    if (buffer->texture) {
        SDL_DestroyTexture(buffer->texture);
    }

    buffer->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, buffer->width, buffer->height);

    buffer->pitch = buffer->width * buffer->bytes_per_pixel;
    int bitmap_memory_size = (buffer->pitch * buffer->height);
    buffer->memory = mmap(0, bitmap_memory_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}


internal void sdl_init_audio(i32 samples_per_second, i32 buffer_size) {
    SDL_AudioSpec audio_settings = {};

    audio_settings.freq = samples_per_second;
    audio_settings.format = AUDIO_S16LSB;
    audio_settings.channels = 2;
    audio_settings.samples = 512;

    SDL_OpenAudio(&audio_settings, 0);

    if (audio_settings.format != AUDIO_S16LSB) {
        LOG_E("Oops! We didn't get AUDIO_S16LSB as our sample format!");
        SDL_CloseAudio();
    }
}


internal void sdl_clear_buffer(sdl_sound_output_t *g_sound_output) {
    SDL_ClearQueuedAudio(1);
}


internal u64 sdl_get_wall_clock() {
    return SDL_GetPerformanceCounter();
}


internal void toggle_full_screen(SDL_Window *window) {
    u32 flags = SDL_GetWindowFlags(window);

    if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
        SDL_SetWindowFullscreen(window, 0);
    } else {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
}


internal sdl_window_dimension_t sdl_get_window_dimension(SDL_Window *window) {
    sdl_window_dimension_t result;

    SDL_GetWindowSize(window, &result.width, &result.height);

    return result;
}


internal void sdl_process_keyboard_message(game_button_state_t *newState, bool32 isDown) {
    if (newState->ended_down != isDown) {
        newState->ended_down = isDown;
        newState->half_transition_count++;
    }
}


internal void sdl_display_buffer_in_window(sdl_offscreen_buffer_t *buffer, SDL_Renderer *renderer, int window_width, int windowHeight) {
    SDL_UpdateTexture(buffer->texture, 0, buffer->memory, buffer->pitch);

    if ((window_width >= buffer->width * 2) && (windowHeight >= buffer->height * 2)) {

        SDL_Rect src_rect = {0, 0, buffer->width, buffer->height};
        SDL_Rect dest_rect = {0, 0, 2 * buffer->width, 2 * buffer->height};
        SDL_RenderCopy(renderer, buffer->texture, &src_rect, &dest_rect);

    } else {

#if 0
        int offset_x = 10;
        int offset_y = 10;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect blackRects[4] = {
            {0, 0, window_width, offset_y},
            {0, offset_y + buffer->height, window_width, windowHeight},
            {0, 0, offset_x, windowHeight},
            {offset_x + buffer->width, 0, window_width, windowHeight}
        };
        SDL_RenderFillRects(renderer, blackRects, ARRAY_COUNT(blackRects));
#else
        int offset_x = 0;
        int offset_y = 0;
#endif

        SDL_Rect src_rect = {0, 0, buffer->width, buffer->height};
        SDL_Rect dest_rect = {offset_x, offset_y, buffer->width, buffer->height};
        SDL_RenderCopy(renderer, buffer->texture, &src_rect, &dest_rect);
    }

    SDL_RenderPresent(renderer);
}


internal void sdl_process_pending_events(sdl_state_t *state, game_controller_input_t *keyboard_controller) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {

        switch (event.type) {

        case SDL_QUIT: {
            g_running = false;
        }
        break;

        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            SDL_Keycode key_code = event.key.keysym.sym;

            bool32 is_down = (event.key.state == SDL_PRESSED);

            if (event.key.repeat == 0) {

                if (key_code == SDLK_w) {
                    sdl_process_keyboard_message(&keyboard_controller->up, is_down);
                } else if (key_code == SDLK_s) {
                    sdl_process_keyboard_message(&keyboard_controller->down, is_down);
                } else if (key_code == SDLK_a) {
                    sdl_process_keyboard_message(&keyboard_controller->left, is_down);
                } else if (key_code == SDLK_d) {
                    sdl_process_keyboard_message(&keyboard_controller->right, is_down);
                } else if (key_code == SDLK_q) {
                    LOG_D("Q");
                } else if (key_code == SDLK_e) {
                    LOG_D("E");
                } else if (key_code == SDLK_UP) {
                    LOG_D("up");
                } else if (key_code == SDLK_DOWN) {
                    LOG_D("down");
                } else if (key_code == SDLK_LEFT) {
                    LOG_D("left");
                } else if (key_code == SDLK_RIGHT) {
                    LOG_D("right");
                } else if (key_code == SDLK_ESCAPE) {
                    LOG_D("esc");
                } else if (key_code == SDLK_SPACE) {
                    sdl_process_keyboard_message(&keyboard_controller->x, is_down);
                }

                if (is_down) {

                    bool32 alt_key_was_down = (event.key.keysym.mod & KMOD_ALT);

                    if ((key_code == SDLK_F4) && alt_key_was_down) {
                        g_running = false;
                    }

                    if ((key_code == SDLK_RETURN) && alt_key_was_down) {

                        SDL_Window *window = SDL_GetWindowFromID(event.window.windowID);

                        if (window) {
                            toggle_full_screen(window);
                        }
                    }
                }
            }
        }
        break;

        case SDL_WINDOWEVENT: {
            switch (event.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED: {
                SDL_Window *window = SDL_GetWindowFromID(event.window.windowID);
                SDL_Renderer *renderer = SDL_GetRenderer(window);
            }
            break;

            case SDL_WINDOWEVENT_FOCUS_GAINED: {
            }
            break;

            case SDL_WINDOWEVENT_EXPOSED: {
                SDL_Window *window = SDL_GetWindowFromID(event.window.windowID);
                SDL_Renderer *renderer = SDL_GetRenderer(window);
                sdl_window_dimension_t dimension = sdl_get_window_dimension(window);
                sdl_display_buffer_in_window(&g_back_buffer, renderer, dimension.width, dimension.height);
            }
            break;
            }
        }
        break;
        }
    }
}


internal f32 sdl_process_game_controller_axis_value(i16 value, i16 dead_zone_threshold) {
    f32 result = 0;

    if (value < dead_zone_threshold) {
        result = (f32)((value + dead_zone_threshold) / (32768.0f - dead_zone_threshold));
    } else if (value > dead_zone_threshold) {
        result = (f32)((value - dead_zone_threshold) / (32767.0f - dead_zone_threshold));
    }

    return result;
}


internal void sdl_process_controller(SDL_GameController *controller, game_controller_input_t *old_controller, game_controller_input_t *new_controller) {

    // TODO(max): Da completare da riga 1436 a riga 1519 del file:
    // https://github.com/KimJorgensen/sdl_handmade/blob/master/code/sdl_handmade.cpp#L1379
    i16 axis_lx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
    i16 axis_ly = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

    i16 axis_rx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
    i16 axis_ry = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);

    i16 trigger_l = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
    i16 trigger_r = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

    new_controller->average_lx = sdl_process_game_controller_axis_value(axis_lx, CONTROLLER_AXIS_LEFT_DEADZONE);
    new_controller->average_ly = sdl_process_game_controller_axis_value(axis_ly, CONTROLLER_AXIS_LEFT_DEADZONE);

    new_controller->average_rx = sdl_process_game_controller_axis_value(axis_rx, CONTROLLER_AXIS_RIGHT_DEADZONE);
    new_controller->average_ry = sdl_process_game_controller_axis_value(axis_ry, CONTROLLER_AXIS_RIGHT_DEADZONE);

    new_controller->average_tl = sdl_process_game_controller_axis_value(trigger_l, CONTROLLER_AXIS_TRIGGER_DEADZONE);
    new_controller->average_tr = sdl_process_game_controller_axis_value(trigger_r, CONTROLLER_AXIS_TRIGGER_DEADZONE);

    f32 threshold = 0.5f;

    // TODO(max): finish process gamepad input buttons with sdl_processGameControllerDigitalButton


    new_controller->is_analog = old_controller->is_analog;

    if ((new_controller->average_ly != 0.0f) || (new_controller->average_lx != 0.0f)) {
        new_controller->is_analog = true;
    }
}


internal sdl_game_code_t sdl_load_game_code() {
    sdl_game_code_t result = {};

    result.initialize = game_initialize;
    result.update_and_render = game_update_and_render;
    result.get_sound_samples = game_get_sound_samples;

    return result;
}


internal f32 sdl_get_seconds_elapsed(u64 start, u64 end) {
    f32 result = ((f32)(end - start) / (f32) g_perf_counter_frequency);
    return result;
}


internal void sdl_fill_sound_buffer(sdl_sound_output_t *g_sound_output, int bytes_to_write, game_sound_output_buffer_t *sound_buffer) {
    SDL_QueueAudio(1, sound_buffer->samples, bytes_to_write);
}


internal void sdl_open_game_controllers() {
    int max_joysticks = SDL_NumJoysticks();
    int controller_index = 0;

    for (int i = 0; i < max_joysticks; i++) {

        if (!SDL_IsGameController(i)) {
            continue;
        }

        if (controller_index >= MAX_CONTROLLERS) {
            break;
        }

        controller_handles[controller_index] = SDL_GameControllerOpen(i);
        SDL_Joystick *joystick_handle = SDL_GameControllerGetJoystick(controller_handles[controller_index]);
        rumble_handles[controller_index] = SDL_HapticOpenFromJoystick(joystick_handle);

        if (SDL_HapticRumbleInit(rumble_handles[controller_index]) != 0) {
            SDL_HapticClose(rumble_handles[controller_index]);
            rumble_handles[controller_index] = 0;
        }

        controller_index++;
    }
}

internal void sdl_close_game_controllers() {
    for (int i = 0; i < MAX_CONTROLLERS; i++) {
        if (controller_handles[i]) {
            if (rumble_handles[i]) {
                SDL_HapticClose(rumble_handles[i]);
            }

            SDL_GameControllerClose(controller_handles[i]);
        }
    }
}


internal game_offscreen_buffer_t get_video_buffer() {
    game_offscreen_buffer_t buffer = {};

    buffer.memory = g_back_buffer.memory;
    buffer.width = g_back_buffer.width;
    buffer.height = g_back_buffer.height;
    buffer.pitch = g_back_buffer.pitch;
    buffer.bytes_per_pixel = g_back_buffer.bytes_per_pixel;

    return buffer;
}



/*****************************************************************************
 *                                  MAIN
 *****************************************************************************/
int main(int argc, char *argv[]) {

    LOG_I("Start");

    // Load game code
    sdl_game_code_t game = sdl_load_game_code();
    g_perf_counter_frequency = SDL_GetPerformanceFrequency();

    sdl_open_game_controllers();

#if RS_SHOW_CURSOR
    g_show_cursor = true;
#else
    g_show_cursor = false;
#endif

    g_running = true;
    g_pause = false;

    // Init SDL
    CHECK_ERR(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_AUDIO));

    // Create the window
    SDL_Window *window = SDL_CreateWindow("RayStorm", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SDL_WIDTH, SDL_HEIGHT, SDL_WINDOW_RESIZABLE);
    CHECK_NULL(window);

    SDL_ShowCursor(g_show_cursor ? SDL_ENABLE : SDL_DISABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    CHECK_NULL(renderer);

    sdl_resize_texture(&g_back_buffer, renderer, SDL_WIDTH, SDL_HEIGHT);

    // Configure refresh rate
    int monitor_refresh_Hz = 60;
    int display_index = SDL_GetWindowDisplayIndex(window);
    SDL_DisplayMode mode = {};
    int display_mode_result = SDL_GetDesktopDisplayMode(display_index, &mode);

    if (display_mode_result == 0 && mode.refresh_rate > 1) {
        monitor_refresh_Hz = mode.refresh_rate;
    }

    f32 game_update_Hz = (f32)(monitor_refresh_Hz / 2.0f);
    f32 target_seconds_per_frame = 1.0f / (f32) game_update_Hz;

    // Configure audio
    bool32 sound_is_valid = false;

    g_sound_output.samples_per_second = 48000;
    g_sound_output.bytes_per_sample = sizeof(i16) * 2;
    g_sound_output.secondary_buffer_size = g_sound_output.samples_per_second * g_sound_output.bytes_per_sample;
    g_sound_output.safety_bytes = (int)(((f32) g_sound_output.samples_per_second * (f32) g_sound_output.bytes_per_sample / game_update_Hz) / 2.0f);
    sdl_init_audio(g_sound_output.samples_per_second, g_sound_output.secondary_buffer_size);
    sdl_clear_buffer(&g_sound_output);
    SDL_PauseAudio(0);

    u32 max_possible_overrun = 8;
    g_audio_samples = (i16 *) calloc(g_sound_output.samples_per_second + max_possible_overrun, g_sound_output.bytes_per_sample);
    CHECK_NULL(g_audio_samples);

    // Audio debug markers
    int DEBUG_time_marker_index = 0;
    DEBUG_sdl_time_marker_t DEBUG_time_markers[30] = {0};

    // Game memory
#if RS_DEBUG
    void *base_address = (void *)Terabytes((u64)2);
#else
    void *base_address = 0;
#endif

    game_memory_t game_memory = {};
    game_memory.permanent_storage_size = Megabytes((u64)256);
    game_memory.transient_storage_size = Gigabytes((u64)1);

    sdl_state_t sdl_state = {};
    sdl_state.total_size = game_memory.permanent_storage_size + game_memory.transient_storage_size;
    sdl_state.game_memory_block = mmap(base_address, (size_t)sdl_state.total_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    game_memory.permanent_storage = sdl_state.game_memory_block;
    game_memory.transient_storage = ((u8 *) game_memory.permanent_storage + game_memory.permanent_storage_size);

    CHECK_NULL(game_memory.permanent_storage);
    CHECK_NULL(game_memory.transient_storage);

    // Keyboard and Joystick inputs
    game_input_t input[2] = {};
    game_input_t *new_input = &input[0];
    game_input_t *old_input = &input[1];

    // Configure timing
    u64 last_counter = sdl_get_wall_clock();
    u64 flip_wall_clock = sdl_get_wall_clock();

    u64 last_cycle_count = _rdtsc();

    // Init game
    CHECK_NULL(game.initialize);
    game_offscreen_buffer_t video_buffer = get_video_buffer();
    game.initialize(&game_memory, &video_buffer);

    // Main loop
    while (g_running) {
        /////////////////////////////////////////////////// Reset inputs data
        new_input->dt_for_frame = target_seconds_per_frame;

        new_input->executable_reloaded = false;

        game_controller_input_t *old_keyboard_controller = get_controller(old_input, 0);
        game_controller_input_t *new_keyboard_controller = get_controller(new_input, 0);

        *new_keyboard_controller = (game_controller_input_t) { 0 }; // TODO(max): check if this even work
        new_keyboard_controller->is_connected = true;

        for (int button_index = 0; button_index < ARRAY_COUNT(new_keyboard_controller->buttons); button_index++) {
            new_keyboard_controller->buttons[button_index].ended_down = old_keyboard_controller->buttons[button_index].ended_down;
        }

        /////////////////////////////////////////////////// Process the inputs
        sdl_process_pending_events(&sdl_state, new_keyboard_controller);

        // If we are in Pause than just skip to the next frame
        if (g_pause) {
            continue;
        }

        // Mouse and keyboard as first joystick
        u32 mouse_state = SDL_GetMouseState(&(new_input->mouse_x), &(new_input->mouse_y));

        // TODO(max): Support mousewheel?
        new_input->mouse_z = 0;

        sdl_process_keyboard_message(&new_input->mouse_buttons[0], mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
        sdl_process_keyboard_message(&new_input->mouse_buttons[1], mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE));
        sdl_process_keyboard_message(&new_input->mouse_buttons[2], mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
        sdl_process_keyboard_message(&new_input->mouse_buttons[3], mouse_state & SDL_BUTTON(SDL_BUTTON_X1));
        sdl_process_keyboard_message(&new_input->mouse_buttons[4], mouse_state & SDL_BUTTON(SDL_BUTTON_X2));

        u32 max_controller_count = MAX_CONTROLLERS;

        if (max_controller_count > (ARRAY_COUNT(new_input->controllers) - 1)) {
            max_controller_count = (ARRAY_COUNT(new_input->controllers) - 1);
        }

        for (u32 controller_index = 0; controller_index < max_controller_count; controller_index++) {
            u32 our_controller_index = controller_index + 1;

            game_controller_input_t *old_controller = get_controller(old_input, our_controller_index);
            game_controller_input_t *new_controller = get_controller(new_input, our_controller_index);

            SDL_GameController *controller = controller_handles[controller_index];

            if (controller && SDL_GameControllerGetAttached(controller)) {

                new_controller->is_connected = true;

                sdl_process_controller(controller, old_controller, new_controller);
            } else {
                // NOTE(max): controller not connected
                new_controller->is_connected = false;
            }
        }

        /////////////////////////////////////////////////// Update and Render
        video_buffer = get_video_buffer();
        CHECK_NULL(game.update_and_render);
        game.update_and_render(&game_memory, new_input, &video_buffer);

        /////////////////////////////////////////////////// Audio
        u64 audio_wall_clock = sdl_get_wall_clock();
        f32 from_begin_to_audio_seconds = sdl_get_seconds_elapsed(flip_wall_clock, audio_wall_clock);

        u32 queued_audio_bytes = SDL_GetQueuedAudioSize(1);

        // TODO(max): WTF
        if (!sound_is_valid) {
            sound_is_valid = true;
        }

        u32 expected_sound_bytes_per_frame = (int)((f32)(g_sound_output.samples_per_second * g_sound_output.bytes_per_sample) / game_update_Hz);

        f32 seconds_left_until_flip = (target_seconds_per_frame - from_begin_to_audio_seconds);
        u32 expected_bytes_until_flip = (u32)((seconds_left_until_flip / target_seconds_per_frame) * (f32) expected_sound_bytes_per_frame);

        i32 bytes_to_write = (expected_sound_bytes_per_frame + g_sound_output.safety_bytes) - queued_audio_bytes;

        if (bytes_to_write < 0) {
            bytes_to_write = 0;
        }

        game_sound_output_buffer_t sound_buffer = {};
        sound_buffer.samples_per_second = g_sound_output.samples_per_second;
        // TODO(max): Align8(bytes_to_write / g_sound_output.bytes_per_sample);
        sound_buffer.sample_count = bytes_to_write / g_sound_output.bytes_per_sample;

        bytes_to_write = sound_buffer.sample_count * g_sound_output.bytes_per_sample;
        sound_buffer.samples = g_audio_samples;

        CHECK_NULL(game.get_sound_samples);
        game.get_sound_samples(&game_memory, &sound_buffer);

#if RS_DEBUG
        DEBUG_sdl_time_marker_t *marker = &DEBUG_time_markers[DEBUG_time_marker_index];
        marker->queued_audio_bytes = queued_audio_bytes;
        marker->output_byte_count = bytes_to_write;
        marker->expected_bytes_until_flip = expected_bytes_until_flip;

#if 0
        u32 audio_latency_bytes = queued_audio_bytes;
        f32 audio_latency_seconds = (((f32) audio_latency_bytes / (f32)g_sound_output.bytes_per_sample) / (f32) g_sound_output.samples_per_second);
        LOG_D("BTW:%u - Latency:%d (%fs)", bytes_to_write, audio_latency_bytes, audio_latency_seconds);
#endif  // 0

#endif // RS_DEBUG

        sdl_fill_sound_buffer(&g_sound_output, bytes_to_write, &sound_buffer);


        /////////////////////////////////////////////////// Frame counter ans sync
        u64 work_counter = sdl_get_wall_clock();
        f32 work_seconds_elapsed = sdl_get_seconds_elapsed(last_counter, work_counter);

        // TODO(max): Not tested yet!!!
        f32 seconds_elapsed_from_frame = work_seconds_elapsed;

        if (seconds_elapsed_from_frame < target_seconds_per_frame) {
            u32 sleep_MS = (u32)(1000.0f * (target_seconds_per_frame - seconds_elapsed_from_frame));

            if (sleep_MS > 0) {
                SDL_Delay(sleep_MS);
            }

            f32 test_seconds_elapsed_for_frame = sdl_get_seconds_elapsed(last_counter, sdl_get_wall_clock());

            if (test_seconds_elapsed_for_frame < target_seconds_per_frame) {
                // NOTE(max): LOG MISSED SLEEP
                LOG_D("Missed sleep");
            }

            while (seconds_elapsed_from_frame < target_seconds_per_frame) {
                seconds_elapsed_from_frame = sdl_get_seconds_elapsed(last_counter, sdl_get_wall_clock());
            }
        } else {
            // TODO(max): MISSED FRAME RATE!
            LOG_D("Missed frame");
        }

        //
        u64 end_counter = sdl_get_wall_clock();
        f32 ms_per_frame = 1000.0f * sdl_get_seconds_elapsed(last_counter, end_counter);
        last_counter = end_counter;

        sdl_window_dimension_t dimension = sdl_get_window_dimension(window);
        sdl_display_buffer_in_window(&g_back_buffer, renderer, dimension.width, dimension.height);

        flip_wall_clock = sdl_get_wall_clock();

        // Swap inputs
        game_input_t *temp = new_input;
        new_input = old_input;
        old_input = temp;

#if 0
        u64 end_cycle_count = __rdtsc();
        u64 cycles_elapsed = end_cycle_count - last_cycle_count;
        last_cycle_count = end_cycle_count;

        f64 FPS = 0.0f;
        f64 MCPF = ((f64)cycles_elapsed / (1000.0f * 1000.0f));

        LOG_D("%.02fms/f,  %.02ff/s,  %.02fmc/f", ms_per_frame, FPS, MCPF);
#endif // 0

#if RS_DEBUG
        DEBUG_time_marker_index++;

        if (DEBUG_time_marker_index == ARRAY_COUNT(DEBUG_time_markers)) {
            DEBUG_time_marker_index = 0;
        }

#endif // RS_DEBUG

    } // while


    sdl_close_game_controllers();
    SDL_Quit();

    return 0;
}
