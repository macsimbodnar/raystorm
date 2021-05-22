#pragma once
#include "common_platform.h"

#define TILE_SIDE  1.0f                 // [metres]

typedef struct {
    u32 w;
    u32 h;
    u8 *tiles_chunk;
} tiles_t;

typedef struct {
    f32 tile_w;
    f32 tile_h;

    u32 num_of_chunks;
    tiles_t *all_chunks;

    tiles_t *current_chunk;
} map_t;

typedef struct {
    f32 x;
    f32 y;
} player_t;

typedef struct {
    map_t       map;
    player_t    player;
} world_t;
