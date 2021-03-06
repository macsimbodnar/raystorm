#pragma once
#include "common_platform.h"
#include "trigonometry.h"

/**
 * World precise position data structure
 * The coordinates are in cartesian coordinate system. 
 * The abs tile is the coordinate of the tile and the offset is the coordinate from the tile left bottom vertex
 */
typedef struct {
    point_i32_t tile;
    point_f32_t offset;
} world_pos_t;

typedef world_pos_t real_pos_t;

typedef struct {
    f32 tile_side_in_meters;
    u32 tile_side_in_pixels;

    u32 width;
    u32 height;

    u32 *tiles;
} tile_map_t;


u8 get_tile_value_in_raw_coordinates(const tile_map_t *map, u32 X, u32 Y);
u8 get_tile_value(const tile_map_t *map, u32 X, u32 Y);
bool32 is_tile_empty(const tile_map_t *map, u32 X, u32 Y);

real_pos_t cart_to_real_pos(const tile_map_t *map, const world_pos_t pos);

void recanonicalize_pos(const tile_map_t *map, world_pos_t *pos);