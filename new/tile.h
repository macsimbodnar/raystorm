#pragma once
#include "common_platform.h"
#include "trigonometry.h"


typedef struct {
    point_u32_t tile_chunk;
    point_u32_t tile_index_in_chunk;
} abs_tile_pos_t;


typedef struct {
    abs_tile_pos_t  abs_tile;
    point_u32_t     offsent_from_tile_center;
} tile_chunk_pos_t;


typedef struct {
    u32 width;
    u32 height;

    u32 *tiles;
} tile_chunk_t;


typedef struct {
    f32 tile_side_in_meters;
    u32 tile_side_in_pixels;

    u32 chunks_count_x;
    u32 chunks_count_y;

    tile_chunk_t *chunks;
} tile_map_t;


u8 get_tile_value(const tile_map_t *map, u32 X, u32 Y);
