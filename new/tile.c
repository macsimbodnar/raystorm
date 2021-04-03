#include "tile.h"
#include "intrinsics.h"

u8 get_tile_value(const tile_map_t *map, u32 X, u32 Y) {
    u8 res = map->tiles[Y * map->width + X];
    return res;
}

real_pos_t cart_to_real_pos(const tile_map_t *map, const cart_pos_t pos) {
    ASSERT(map->height > pos.tile.Y);
    ASSERT(map->tile_side_in_meters >= pos.offset.Y);

    real_pos_t res;
    res.tile.X = pos.tile.X;
    res.offset.X = pos.offset.X;
    
    // Invert the the Y axis
    res.tile.Y = map->height - pos.tile.Y - 1;
    res.offset.Y = map->tile_side_in_meters - pos.offset.Y;

    return res;
}


void recanonicalize_pos(const tile_map_t *map, cart_pos_t *pos) {

    // NOTE(max): the offset must have sign so we can handle negative offsets
    i32 offset_x = floor_f32_to_i32(pos->offset.X / map->tile_side_in_meters);
    i32 offset_y = floor_f32_to_i32(pos->offset.Y / map->tile_side_in_meters);

    pos->tile.X += offset_x;
    pos->tile.Y += offset_y;

    pos->offset.X -= offset_x * map->tile_side_in_meters;
    pos->offset.Y -= offset_y * map->tile_side_in_meters;
}
