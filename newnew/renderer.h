#pragma once
#include "common_platform.h"
#include "world.h"

void draw_rectangle(game_offscreen_buffer_t *buffer, const rect_t rect, const color_t color);


void draw_world(game_offscreen_buffer_t *buffer, const world_t *world);