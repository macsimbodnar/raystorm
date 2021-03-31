#include "tile.h"

u8 get_tile_value(const tile_map_t *map, u32 X, u32 Y) {
    u8 res = map->chunks->tiles[Y * map->chunks->width + X];
    return res;
}