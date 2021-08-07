#include "raystorm.h"
#include "intrinsics.h"
#include "trigonometry.h"
#include "log.h"


#define MINIMAP_W                       150
#define MINIMAP_H                       100
#define MINIMAP_BYTES_PER_PIXEL         4
#define MINIMAP_PIXELS_PER_TILE         10
global_var u8 minimap_mem[MINIMAP_W * MINIMAP_H * MINIMAP_BYTES_PER_PIXEL];
global_var game_offscreen_buffer_t g_minimap = {
    .memory = minimap_mem,
    .width = MINIMAP_W,
    .height = MINIMAP_H,
    .pitch = MINIMAP_W * MINIMAP_BYTES_PER_PIXEL,
    .bytes_per_pixel = MINIMAP_BYTES_PER_PIXEL
};

global_var color_t RED =        {1.0f, .0f, .0f};
global_var color_t GREEN =      {.0f, 1.0f, .0f};
global_var color_t BLUE =       {.0f, .0f, 1.0f};
global_var color_t WHITE =      {1.0f, 1.0f, 1.0f};
global_var color_t BLACK =      {.0f, .0f, .0f};
global_var color_t PURPLE =     {1.0f, .0f, 1.0f};

global_var i32 g_player_tail_x = 0;
global_var i32 g_player_tail_y = 0;
global_var i32 g_player_rel_x = 0;
global_var i32 g_player_rel_y = 0;
global_var f32 g_player_angle = .0f;


// #define MAP_W       24
// #define MAP_H       24
// u8 map_walls[MAP_W * MAP_H] = {                  // Map of walls 24 * 24
//     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
// };

#define MAP_W       8
#define MAP_H       8
u8 map_walls[MAP_W * MAP_H] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};


// internal void draw_line(game_offscreen_buffer_t *buffer, point_i32_t A, point_i32_t B, const color_t color) {
//     // Kinda bresenham algorithm
//     i32 min_x = (i32)A.X;
//     i32 min_y = (i32)A.Y;
//     i32 max_x = (i32)B.X;
//     i32 max_y = (i32)B.Y;

//     if (A.X >= B.X && A.Y >= B.Y) {
//         min_x = B.X;
//         min_y = B.Y;
//         max_x = A.X;
//         max_y = A.Y;
//     } else {
//         min_x = A.X;
//         min_y = A.Y;
//         max_x = B.X;
//         max_y = B.Y;
//     }

//     bool32 inverse = false;

//     if ((A.X > B.X && A.Y < B.Y) || (A.X < B.X && A.Y > B.Y)) {
//         inverse = true;
//     }

//     u32 color_32 = ((round_f32_to_u32(color.R * 255.0f) << 16) | (round_f32_to_u32(color.G * 255.0f) << 8) | (round_f32_to_u32(color.B * 255.0f) << 0));
//     i32 delta_x = abs_i32(max_x - min_x);
//     i32 delta_y = abs_i32(max_y - min_y);
//     i32 x = min_x;
//     i32 y = min_y;
//     i32 end_x = max_x;
//     i32 end_y = max_y;
//     i32 p;
//     u32 *pixel;

//     if (delta_x > delta_y) {

//         p = 2 * delta_y - delta_x;

//         if (inverse) {
//             while (x < end_x) {
//                 pixel = (u32 *)(((u8 *) buffer->memory) + x * buffer->bytes_per_pixel + y * buffer->pitch);
//                 *pixel = color_32;

//                 if (p >= 0) {
//                     --y;
//                     p += 2 * delta_y - 2 * delta_x;
//                 } else {
//                     p += 2 * delta_y;
//                 }

//                 ++x;
//             }
//         } else {
//             while (x < end_x) {
//                 pixel = (u32 *)(((u8 *) buffer->memory) + x * buffer->bytes_per_pixel + y * buffer->pitch);
//                 *pixel = color_32;

//                 if (p >= 0) {
//                     ++y;
//                     p += 2 * delta_y - 2 * delta_x;
//                 } else {
//                     p += 2 * delta_y;
//                 }

//                 ++x;
//             }
//         }

//     } else {

//         p = 2 * delta_x - delta_y;

//         if (inverse) {
//             while (y < end_y) {
//                 pixel = (u32 *)(((u8 *) buffer->memory) + x * buffer->bytes_per_pixel + y * buffer->pitch);
//                 *pixel = color_32;

//                 if (p >= 0) {
//                     --x;
//                     p = p + 2 * delta_x - 2 * delta_y;
//                 } else {
//                     p = p + 2 * delta_x;
//                 }

//                 y = y + 1;
//             }
//         } else {
//             while (y < end_y) {
//                 pixel = (u32 *)(((u8 *) buffer->memory) + x * buffer->bytes_per_pixel + y * buffer->pitch);
//                 *pixel = color_32;

//                 if (p >= 0) {
//                     x = x + 1;
//                     p = p + 2 * delta_x - 2 * delta_y;
//                 } else {
//                     p = p + 2 * delta_x;
//                 }

//                 y = y + 1;
//             }
//         }
//     }
// }


internal void draw_rectangle(game_offscreen_buffer_t *buffer, rect_t *rect, color_t color) {
    i32 min_x = rect->x;
    i32 min_y = rect->y;
    i32 max_x = rect->x + rect->width;
    i32 max_y = rect->y + rect->height;

    if (min_x < 0) {
        min_x = 0;
    }

    if (min_y < 0) {
        min_y = 0;
    }

    if (max_x > buffer->width) {
        max_x = buffer->width;
    }

    if (max_y > buffer->height) {
        max_y = buffer->height;
    }

    u32 color_32 = ((round_f32_to_u32(color.R * 255.0f) << 16) | (round_f32_to_u32(color.G * 255.0f) << 8) | (round_f32_to_u32(color.B * 255.0f) << 0));

    u8 *row = (((u8 *) buffer->memory) + min_x * buffer->bytes_per_pixel + min_y * buffer->pitch);

    for (int y = min_y; y < max_y; y++) {
        u32 *pixel = (u32 *) row;

        for (int x = min_x; x < max_x; x++) {
            *pixel++ = color_32;
        }

        row += buffer->pitch;
    }
}


internal void draw_buffer(game_offscreen_buffer_t *buffer, i32 X, i32 Y, game_offscreen_buffer_t *buffer_to_draw) {
    i32 min_x = X;
    i32 min_y = Y;
    i32 max_x = min_x + buffer_to_draw->width;
    i32 max_y = min_y + buffer_to_draw->height;

    if (min_x < 0) {
        min_x = 0;
    }

    if (min_y < 0) {
        min_y = 0;
    }

    if (max_x > buffer->width) {
        max_x = buffer->width;
    }

    if (max_y > buffer->height) {
        max_y = buffer->height;
    }

    u8 *row = (((u8 *) buffer->memory) + min_x * buffer->bytes_per_pixel + min_y * buffer->pitch);
    u8 *source_row = buffer_to_draw->memory;

    for (int y = min_y; y < max_y; y++) {
        u32 *pixel = (u32 *) row;
        u32 *source_pixel = (u32 *) source_row;

        for (int x = min_x; x < max_x; x++) {
            *pixel = *source_pixel;
            ++pixel;
            ++source_pixel;
        }

        row += buffer->pitch;
        source_row += buffer_to_draw->pitch;
    }
}


internal void draw_minimap(game_state_t *game, game_offscreen_buffer_t *buffer) {

    i32 x_offset, y_offset;
    f32 r, g, b;

    i32 pixels_per_tile = MINIMAP_PIXELS_PER_TILE;
    u32 how_many_columns = MINIMAP_W / MINIMAP_PIXELS_PER_TILE;
    u32 how_many_rows = MINIMAP_H / MINIMAP_PIXELS_PER_TILE;

    i32 map_start_x = g_player_tail_x - how_many_columns / 2 - 1;
    i32 map_start_y = g_player_tail_y - how_many_rows / 2 - 1;

    if (map_start_x < 0) {
        map_start_x = 0;
    }

    if (map_start_y < 0) {
        map_start_y = 0;
    }

    i32 map_end_x = g_player_tail_x + how_many_columns / 2 + 1;
    i32 map_end_y = g_player_tail_y + how_many_rows / 2 + 1;

    if (map_end_x > MAP_W) {
        map_end_x = MAP_W;
    }

    if (map_end_y > MAP_H) {
        map_end_y = MAP_H;
    }

    // map_start_x = 0;
    // map_start_y = 0;
    // map_end_x = MAP_W;
    // map_end_y = MAP_H;

    // Clear g_minimap
    rect_t full_minimap = {0, 0, g_minimap.width, g_minimap.height};

    draw_rectangle(&g_minimap, &full_minimap, GREEN);

    for (i32 y = map_start_y; y < map_end_y; ++y) {
        for (i32 x = map_start_x; x < map_end_x; ++x) {
            color_t color;

            if (map_walls[y * MAP_W + x] > 0) {
                color = WHITE;
            } else {
                color = BLACK;
            }

            x_offset = ((g_minimap.width / 2) - (g_player_tail_x * pixels_per_tile)) + x * pixels_per_tile;
            y_offset = ((g_minimap.height / 2) - (g_player_tail_y * pixels_per_tile)) + y * pixels_per_tile;

            rect_t rect = {x_offset, y_offset, pixels_per_tile, pixels_per_tile};
            draw_rectangle(&g_minimap, &rect, color);
        }
    }

    // Draw player
    f32 screen_center_x = 0.5f * (f32)g_minimap.width;
    f32 screen_center_y = 0.5f * (f32)g_minimap.height;
    f32 player_size = .75f * pixels_per_tile;
    f32 player_half = player_size / 2;
    rect_t rec = {screen_center_x - player_half, screen_center_y - player_half, player_size, player_size};
    draw_rectangle(&g_minimap, &rec, RED);

    // Draw g_minimap on screen
    u32 minimap_x = 10;
    u32 minimap_y = buffer->height - g_minimap.height - 10;
    draw_buffer(buffer, minimap_x, minimap_y, &g_minimap);

    // LOG_I("P(%d, %d)   Min(%d, %d)    Max(%d, %d)", (int)g_player_tail_x, (int)g_player_tail_y, (int)map_start_x, (int)map_start_y, (int)map_end_x, (int)map_end_y);
}


internal void update_player(game_input_t *input) {
    game_controller_input_t *controller = &input->controllers[0];

    if (controller->up.ended_down) {
        g_player_rel_y -= 1;
    }

    if (controller->down.ended_down) {
        g_player_rel_y += 1;
    }

    if (controller->left.ended_down) {
        g_player_rel_x -= 1;
    }

    if (controller->right.ended_down) {
        g_player_rel_x += 1;
    }


    if (g_player_rel_y > 5) {
        g_player_rel_y = 0;
        ++g_player_tail_y;
    }

    if (g_player_rel_y < -5) {
        g_player_rel_y = 0;
        --g_player_tail_y;
    }

    if (g_player_rel_x < -5) {
        g_player_rel_x = 0;
        --g_player_tail_x;
    }

    if (g_player_rel_x > 5) {
        g_player_rel_x = 0;
        ++g_player_tail_x;
    }
}


#define FILED_OF_VIEW 60
internal void draw_rays(game_offscreen_buffer_t *buffer) {

    f32 ray_angle = g_player_angle - DR * 30;

    if (ray_angle < 0) {
        ray_angle += 2 * Pi32;
    }

    if (ray_angle > 2 * Pi32) {
        ray_angle -= 2 * Pi32;
    }


    for (u32 ray = 0; ray < FILED_OF_VIEW; ++ray) {
        i32 map_x;
        i32 map_y;
        i32 map_p;
        f32 ray_x;
        f32 ray_y;
        f32 x_offset;
        f32 y_offset;
        i32 depth_of_field;
        f32 angle_tan;
        f32 n_tan;
        f32 distance_h;
        f32 distance_v;
        f32 distance_t;
        f32 hor_x;
        f32 hor_y;
        f32 ver_x;
        f32 ver_y;
        // ----------- Horizontal lines -----------
        depth_of_field = 0;
        angle_tan = -1 / tanf(ray_angle);
        distance_h = 10000000;
        hor_x = g_player_tail_x;
        hor_y = g_player_tail_y;

        if (ray_angle > Pi32) {
            // Looking down
            ray_y = (((int)g_player_tail_y / 64) * 64) - 0.0001;
            ray_x = (g_player_rel_y - ray_y) * angle_tan + g_player_tail_x;
            y_offset = -64;
            x_offset = -y_offset * angle_tan;
        }

        if (ray_angle < Pi32) {
            // Looking up
            ray_y = (((int)g_player_tail_y / 64) * 64) + 64;
            ray_x = (g_player_tail_y - ray_y) * angle_tan + g_player_tail_x;
            y_offset = 64;
            x_offset = -y_offset * angle_tan;
        }

        if (ray_angle == 0 || ray_angle == Pi32) {
            // Looking straight left or right
            ray_x = g_player_tail_x;
            ray_y = g_player_tail_y;
            depth_of_field = 8;
        }

        while (depth_of_field < 8) {
            map_x = (int) ray_x / 64;
            map_y = (int) ray_y / 64;
            map_p = map_y * map_x + map_x;

            if (map_p > 0 && map_p < map_x * map_y && map_walls[map_p] == 1) {
                // Hit wall
                hor_x = ray_x;
                hor_y = ray_y;
                distance_h = distf(g_player_tail_x, g_player_tail_x, hor_x, hor_y, ray_angle);
                depth_of_field = 8;
            } else {
                // Next line
                ray_x += x_offset;
                ray_y += y_offset;
                depth_of_field += 1;
            }
        }

        // -----------  Vertical lines -----------
        depth_of_field = 0;
        n_tan = - tanf(ray_angle);
        distance_v = 10000000;
        ver_x = g_player_tail_x;
        ver_y = g_player_tail_y;

        if (ray_angle > P2 && ray_angle < P3) {
            // Looking left
            ray_x = (((int)g_player_tail_x / 64) * 64) - 0.0001;
            ray_y = (g_player_tail_x - ray_x) * n_tan + g_player_tail_y;
            x_offset = -64;
            y_offset = -x_offset * n_tan;
        }

        if (ray_angle < P2 || ray_angle > P3) {
            // Looking right
            ray_x = (((int)g_player_tail_x / 64) * 64) + 64;
            ray_y = (g_player_tail_x - ray_x) * n_tan + g_player_tail_y;
            x_offset = 64;
            y_offset = -x_offset * n_tan;
        }

        if (ray_angle == 0 || ray_angle == Pi32) {
            // Looking straight up or down
            ray_x = g_player_tail_x;
            ray_y = g_player_tail_y;
            depth_of_field = 8;
        }

        while (depth_of_field < 8) {
            map_x = (int) ray_x / 64;
            map_y = (int) ray_y / 64;
            map_p = map_y * MAP_W + map_x;

            if (map_p > 0 && map_p < map_x * map_y && map_walls[map_p] == 1) {
                // Hit wall
                ver_x = ray_x;
                ver_y = ray_y;
                distance_v = distf(g_player_tail_x, g_player_tail_x, ver_x, ver_y, ray_angle);
                depth_of_field = 8;
            } else {
                // Next line
                ray_x += x_offset;
                ray_y += y_offset;
                depth_of_field += 1;
            }
        }

        // ----------- DRAW -----------

        // Set the ray to the shorter
        if (distance_v < distance_h) {
            ray_x = ver_x;
            ray_y = ver_y;
            distance_t = distance_v;
        }

        if (distance_h < distance_v) {
            ray_x = hor_x;
            ray_y = hor_y;
            distance_t = distance_h;
        }

        // Draw 3D
        float ca = g_player_angle - ray_angle;

        color_t color;

        if (ca < 0) {
            ca += 2 * Pi32;
            color = (color_t) {0.9f, .0f, .0f};
        }

        if (ca > 2 * Pi32) {
            ca -= 2 * Pi32;
            color = (color_t) {0.6f, .0f, .0f};
        }

        distance_t = distance_t *cosf(ca);                       // Fix fisheye

        int tile_size = 64;
        float line_h = (tile_size * 320) / distance_t;      // Line height

        if (line_h > 320) {
            line_h = 320;
        }

        float line_o = 160 - line_h / 2;                // Line offset

        // Have to split in 2 because on my machine the max line width is less then 8
        // glLineWidth(4);
        // glBegin(GL_LINES);
        // int l = r * 8 + 530 - 2;
        // glVertex2i(l, line_o);
        // glVertex2i(l, line_h + line_o);
        // glEnd();

        // glLineWidth(4);
        // glBegin(GL_LINES);
        // glVertex2i(l + 4, line_o);
        // glVertex2i(l + 4, line_h + line_o);
        // glEnd();

        rect_t rec = {ray * 8, line_o, 8, line_h + line_o};
        draw_rectangle(buffer, &rec, color);

        // void draw_rect(size_t width, size_t x_pos, size_t top_y, size_t bottom_y, rgba_t rgba)
        // draw_rect(8, r * 8, line_o, line_h + line_o, rgb);

        // Update the ray angle
        ray_angle += DR;

        if (ray_angle < 0) {
            ray_angle += 2 * Pi32;
        }

        if (ray_angle > 2 * Pi32) {
            ray_angle -= 2 * Pi32;
        }
    }
}


// void game_initialize(game_memory_t *memory, game_offscreen_buffer_t *buffer)
GAME_INITIALIZE(game_initialize) {
    game_state_t *game = (game_state_t *) memory->permanent_storage;
}


// void game_update_and_render(game_memory_t *memory, game_input_t *input, game_offscreen_buffer_t *buffer)
GAME_UPDATE_AND_RENDER(game_update_and_render) {
    game_state_t *game = (game_state_t *) memory->permanent_storage;

    // Clear screen
    rect_t rec = {.0f, .0f, buffer->width, buffer->height};
    draw_rectangle(buffer, &rec, PURPLE);

    // Update the player pos
    update_player(input);

    // Draw the 3d scene
    draw_rays(buffer);

    // Draw the minimap
    draw_minimap(game, buffer);
}



/**************************************************************************************************
 *                                             SOUND
 *************************************************************************************************/
void game_output_sound(game_state_t *game_state, game_sound_output_buffer_t *buffer, int tone_Hz) {
    // i16 toneVolume = 3000;
    // f32 wavePeriod = (f32)buffer->samples_per_second / tone_Hz;

    i16 *sample_out = buffer->samples;

    for (int i = 0; i < buffer->sample_count; i++) {
        i16 sample_value = 0;
        *sample_out++ = sample_value;
        *sample_out++ = sample_value;
    }
}


// void game_get_sound_samples(game_memory_t *memory, game_sound_output_buffer_t *sound_buffer)
GAME_GET_SOUND_SAMPLES(game_get_sound_samples) {
    game_state_t *game_state = (game_state_t *) memory->permanent_storage;
    game_output_sound(game_state, sound_buffer, 400);
}