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
    u32 *tiles;
} tile_chunk_t;


typedef struct {
    f32 tile_side_in_meters;

    u32 chunks_count_x;
    u32 chunks_count_y;

    tile_chunk_t *chunks;
} tile_map_t;



tile_chunk_t *get_tile_chunk(tile_map_t *tile_map, point_u32_t tile_chunk_pos) {
    tile_chunk_t *tile_chunk = 0;

    if ((tile_chunk_pos.X >= 0) && (tile_chunk_pos.X < tile_map->chunks_count_x) &&
            (tile_chunk_pos.Y >= 0) && (tile_chunk_pos.Y < tile_map->chunks_count_y)) {

        tile_chunk = &(tile_map->chunks[
                    tile_map->chunks_count_y * tile_map->chunks_count_x +
                    tile_chunk_pos.Y * tile_map->chunks_count_x +
                    tile_chunk_pos.X]);
    }

    return tile_chunk;
}


u32 get_tile_value(tile_map_t *tile_map, abs_tile_pos_t abs_tile) {
    point_u32_t chunk_pos = get_chunk_position_for(tile_map, abs_tile);
}