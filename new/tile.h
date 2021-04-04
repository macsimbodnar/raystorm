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
} cart_pos_t;

typedef cart_pos_t real_pos_t;

typedef struct {
    f32 tile_side_in_meters;
    u32 tile_side_in_pixels;

    u32 width;
    u32 height;

    u32 *tiles;
} tile_map_t;


u8 get_tile_value(const tile_map_t *map, u32 X, u32 Y);

real_pos_t cart_to_real_pos(const tile_map_t *map, const cart_pos_t pos);

void recanonicalize_pos(const tile_map_t *map, cart_pos_t *pos);