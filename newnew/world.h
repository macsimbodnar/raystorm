#pragma once
#include "common_platform.h"

#define TILE_SIDE               1.0f                 // [metres]
#define ONE_METER_IN_PIXELS     20.0f

typedef struct {
    f32 X;
    f32 Y;
} position_t;

typedef struct {
    u32 W;
    u32 H;

    f32 side_in_meters;

    u8 *tiles;
} tiles_t;

typedef struct {

    u32 num_of_chunks;
    tiles_t *all_chunks;

    tiles_t *current_chunk;
} map_t;

typedef struct {
    f32         W;
    f32         H;
    position_t  pos;
    f32         angle;      // [radiants]
} player_t;

typedef struct {
    position_t pos;
} camera_t;

typedef struct {
    f32         meter_to_pixel_multiplier;

    map_t       map;
    player_t    player;
    camera_t    camera;
} world_t;


void update_world(world_t *world, const game_input_t *input);