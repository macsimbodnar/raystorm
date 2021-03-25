#define FILED_OF_VIEW       60                  // Degrees of view

global_var i32 g_map_width = 8;
global_var i32 g_map_height = 8;
global_var i32 g_tile_size = 64;
global_var i32 g_player_x = 300;
global_var i32 g_player_y = 300;
global_var f32 g_player_angle = .0f;
global_var i32 g_player_half_thick = 10;





// internal i32 round_f32_to_i32(f32 r32) {
//     i32 result = (i32) roundf(r32);
//     return result;
// }


internal void draw_player(game_offscreen_buffer_t *buffer) {
    rect_t rec = {g_player_x - g_player_half_thick, g_player_y - g_player_half_thick, g_player_half_thick, g_player_half_thick};
    draw_rectangle(buffer, &rec, 1.0f, .0f, .0f);
}


internal f32 dist(f32 ax, f32 ay, f32 bx, f32 by, f32 angle) {
    return (bx - ax) * cos(angle) + (by - ay) * sin(angle);
}


internal void old_draw_rays(game_offscreen_buffer_t *buffer) {
    int r, mx, my, mp, dof;
    float rx, ry, ra, xo, yo, dist_t;

    ra = g_player_angle - DR * 30;

    if (ra < 0) {
        ra += 2 * Pi32;
    }

    if (ra > 2 * Pi32) {
        ra -= 2 * Pi32;
    }

    for (r = 0; r < 60; r++) {
        float red = .0f;
        float green = .0f;
        float blue = .0f;

        int vmt = 0;        // Vertival g_map_walls texture
        int hmt = 0;        // Horizontal g_map_walls texture

        // ----------- Horizontal lines -----------
        dof = 0;
        float a_tan =  -1 / tan(ra);
        float dist_h = 1000000;
        float hx = g_player_x;
        float hy = g_player_y;

        if (ra > Pi32) {
            // Looking down
            ry = (((int)g_player_y / 64) * 64) - 0.0001;
            rx = (g_player_y - ry) * a_tan + g_player_x;
            yo = -64;
            xo = -yo * a_tan;
        }

        if (ra < Pi32) {
            // Looking up
            ry = (((int)g_player_y / 64) * 64) + 64;
            rx = (g_player_y - ry) * a_tan + g_player_x;
            yo = 64;
            xo = -yo * a_tan;
        }

        if (ra == 0 || ra == Pi32) {
            // Looking straight left or right
            rx = g_player_x;
            ry = g_player_y;
            dof = 8;
        }

        while (dof < 8) {
            mx = (int) rx / 64;
            my = (int) ry / 64;
            mp = my * g_map_width + mx;

            if (mp > 0 && mp < g_map_width * g_map_height && g_map_walls[mp] > 0) {
                // Hit wall
                hmt = g_map_walls[mp] - 1;
                hx = rx;
                hy = ry;
                dist_h = dist(g_player_x, g_player_y, hx, hy, ra);
                // dist_h = dist(hx - g_player_x, hy - g_player_y, ra);
                dof = 8;
            } else {
                // Next line
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }

        // Draw
        // glColor3f(0, 1, 0);
        // glLineWidth(9);
        // glBegin(GL_LINES);
        // glVertex2i(g_player_x, g_player_y);
        // glVertex2i(rx, ry);
        // glEnd();

        // -----------  Vertical lines -----------
        dof = 0;
        float n_tan =  - tan(ra);
        float dist_v = 1000000;
        float vx = g_player_x;
        float vy = g_player_y;

        if (ra > P2 && ra < P3) {
            // Looking left
            rx = (((int)g_player_x / 64) * 64) - 0.0001;
            ry = (g_player_x - rx) * n_tan + g_player_y;
            xo = -64;
            yo = -xo * n_tan;
        }

        if (ra < P2 || ra > P3) {
            // Looking right
            rx = (((int)g_player_x / 64) * 64) + 64;
            ry = (g_player_x - rx) * n_tan + g_player_y;
            xo = 64;
            yo = -xo * n_tan;
        }

        if (ra == 0 || ra == Pi32) {
            // Looking straight up or down
            rx = g_player_x;
            ry = g_player_y;
            dof = 8;
        }

        while (dof < 8) {
            mx = (int) rx / 64;
            my = (int) ry / 64;
            mp = my * g_map_width + mx;

            if (mp > 0 && mp < g_map_width * g_map_height && g_map_walls[mp] > 0) {
                // Hit wall
                vmt = g_map_walls[mp] - 1;
                vx = rx;
                vy = ry;
                dist_v = dist(g_player_x, g_player_y, vx, vy, ra);
                // dist_v = dist(vx - g_player_x, vy - g_player_y, ra);
                dof = 8;
            } else {
                // Next line
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }

        // ----------- DRAW -----------

        float shade = 1.0f;
        float ray_r_color = 1.0f;

        // Set the ray to the shorter
        if (dist_v < dist_h) {
            hmt = vmt;
            rx = vx;
            ry = vy;
            dist_t = dist_v;
        }

        if (dist_h < dist_v) {
            shade = 0.5f;
            rx = hx;
            ry = hy;
            dist_t = dist_h;
            ray_r_color = ray_r_color * shade;
        }

        // Draw 2D rays
        // rect_t rect = {g_player_x, g_player_y, rx, ry};
        // draw_rectangle(buffer, &rect, ray_r_color, .0f, .0f);

        // Draw 3D
        float ca = g_player_angle - ra;

        if (ca < 0) {
            ca += 2 * Pi32;
        }

        if (ca > 2 * Pi32) {
            ca -= 2 * Pi32;
        }

        dist_t = dist_t *cos(ca);                       // Fix fisheye

        float line_h = (g_tile_size * 320) / dist_t;      // Line height

        float ty_step = 32.0 / (float)line_h;
        float ty_off = 0;

        if (line_h > 320) {
            ty_off = (line_h - 320) / 2.0f;
            line_h = 320;
        }

        float line_off = 160 - line_h / 2;              // Line offset

        float ty = ty_off * ty_step + hmt * 32;         // Texture Y value
        float tx;                                       // Texture X value

        if (shade == 1.0f) {   // Draw the textures in the wall in front and backward
            tx = (int)(ry / 2.0f) % 32;

            if (ra > P2 && ra < P3) {
                tx = 31 - tx;
            }
        } else {            // Draw the textures in the left and right wall
            tx = (int)(rx / 2.0f) % 32;

            if (ra < Pi32) {
                // Flip the texture if we looking backward
                tx = 31 - tx;
            }
        }

        for (int y = 0; y < line_h; ++y) {
            // Draw walls

            float c = g_all_textures[((int)(ty) * 32) + (int)tx] * shade;

            // Set different colors for different walls

            if (hmt == 0) {
                red = c;
                green = c / 2.0;
                blue = c / 2.0;
                // Checkerboard red
            } else if (hmt == 1) {
                red = c;
                green = c;
                blue = c / 2.0;
                // Brick yellow
            } else if (hmt == 2) {
                red = c / 2.0;
                green = c / 2.0;
                blue = c;
                // Window blue
            } else if (hmt == 3) {
                red = c  / 2.0;
                green = c;
                blue = c / 2.0;
                // Door green
            }

            // glPointSize(8);
            // glBegin(GL_POINTS);
            // glVertex2i(r * 8 + 530, y + line_off);
            // glEnd();

            rect_t rect = {r * 8 + 530, y + line_off, 8, 8};
            draw_rectangle(buffer, &rect, red, green, blue);
            ty += ty_step;
        }

        for (int y = line_off + line_h; y < 320; ++y) {
            // Draw floor
            float dy = y - (320 / 2.0);
            float angle = g_player_angle - ra;

            if (angle < 0) {
                angle += 2 * Pi32;
            }

            if (angle > 2 * Pi32) {
                angle -= 2 * Pi32;
            }

            float ra_fix = cos(angle);

            float tx = g_player_x / 2 + cos(ra) * 158 * 32 / dy / ra_fix;
            float ty = g_player_y / 2 + sin(ra) * 158 * 32 / dy / ra_fix;

            int mp = mapf[(int)(ty / 32.0) * g_map_width + (int)(tx / 32.0)] * 32 * 32;
            float c = g_all_textures[((int)(ty) & 31) * 32 + ((int)(tx) & 31) + mp] * 0.7;

            red = c / 1.3;
            green = c / 1.3;
            blue = c;

            rect_t rect = {r * 8 + 530, y, 8, 8};
            draw_rectangle(buffer, &rect, red, green, blue);

            // Draw ceiling
            mp = mapc[(int)(ty / 32.0) * g_map_width + (int)(tx / 32.0)] * 32 * 32;
            c = g_all_textures[((int)(ty) & 31) * 32 + ((int)(tx) & 31) + mp] * 0.7;

            red = c / 2.0;
            green = c / 1.2;
            blue = c / 2.0;
            rect_t rect2 = {r * 8 + 530, 320 - y, 8, 8};
            draw_rectangle(buffer, &rect2, red, green, blue);
        }

        // Update the ray angle
        ra += DR;

        if (ra < 0) {
            ra += 2 * Pi32;
        }

        if (ra > 2 * Pi32) {
            ra -= 2 * Pi32;
        }
    }
}


internal void draw_rays(game_offscreen_buffer_t *buffer) {

    // // Calculate the angle of the first ray
    // f32 x_offset;
    // f32 y_offset;
    // f32 ray_x;
    // f32 ray_y;
    // f32 ray_angle = g_player_angle - (DR * FILED_OF_VIEW / 2);
    // f32 angle_tan;
    // f32 distance;
    // int depth_of_field;
    // int v_wall_texture;
    // int h_wall_texture;
    // int mx, my, mp; // map

    // // Normalize the angle
    // if (ray_angle < 0) {
    //     ray_angle += 2 * Pi32;
    // }

    // if (ray_angle > 2 * Pi32) {
    //     ray_angle -= 2 * Pi32;
    // }

    // // Loop on each ray
    // for (int ray = 0; ray < FILED_OF_VIEW; ++ray) {
    //     v_wall_texture = 0;
    //     h_wall_texture = 0;
    //     depth_of_field = 0;
    //     angle_tan = -1 / tan(ray_angle);
    //     distance = 1000000.0f;              // Set the distance to really big

    //     f32 horizontal_x = g_player_x;
    //     f32 horizontal_y = g_player_y;

    //     if (ray_angle > Pi32) {
    //         // Looking forward

    //         // TODO(max): wtf is this?
    //         ray_y = (((int)g_player_y / g_tile_size) * g_tile_size) - 0.0001;
    //         ray_x = (g_player_y - ray_y) * angle_tan + g_player_x;
    //         y_offset = -g_tile_size;
    //         x_offset = -y_offset * angle_tan;
    //     }

    //     if (ray_angle < Pi32) {
    //         // Looking backward
    //         ray_y = (((int)g_player_y / g_tile_size) * g_tile_size) + g_tile_size;
    //         ray_x = (g_player_y - ray_y) * angle_tan + g_player_x;
    //         y_offset = g_tile_size;
    //         x_offset = -y_offset * angle_tan;
    //     }

    //     if (ray_angle == 0 || ray_angle == Pi32) {
    //         // Looking straight left or right
    //         ray_x = g_player_x;
    //         ray_y = g_player_y;

    //         // TODO(max): wtf is this?
    //         depth_of_field = g_map_width;
    //     }

    //     // while (depth_of_field < g_map_width) {
    //     //     mx = (int) ray_x / 64;
    //     //     my = (int) ray_y / 64;
    //     //     mp = my * g_map_width + mx;

    //     //     if (mp > 0 && mp < g_map_width * g_map_height && g_map_walls[mp] > 0) {
    //     //         // Hit wall
    //     //         hmt = g_map_walls[mp] - 1;
    //     //         hx = rx;
    //     //         hy = ry;
    //     //         dist_h = dist(g_player_x, g_player_y, hx, hy, ra);
    //     //         // dist_h = dist(hx - g_player_x, hy - g_player_y, ra);
    //     //         dof = 8;
    //     //     } else {
    //     //         // Next line
    //     //         rx += xo;
    //     //         ry += yo;
    //     //         dof += 1;
    //     //     }
    //     // }
    // }
}





internal void update_word(game_memory_t *memory, game_input_t *input) {

    for (u32 i = 0; i < ARRAY_COUNT(input->controllers); ++i) {
        game_controller_input_t *controller = get_controller(input, i);

        if (controller->is_analog) {
            // NOTE(max): analog
        } else {
            // NOTE(max): digital

        }
    }

    // We use only the first controller
    // game_controller_input_t *controller = &input->controllers[0];

    // if (controller->up.ended_down) {
    //     g_player_y -= 1;
    // }

    // if (controller->down.ended_down) {
    //     g_player_y += 1;
    // }

    // if (controller->left.ended_down) {
    //     g_player_angle -= 0.03;

    //     if (g_player_angle < 0) {
    //         g_player_angle += 2 * Pi32;
    //     }
    // }

    // if (controller->right.ended_down) {
    //     g_player_angle += 0.03;

    //     if (g_player_angle > 2 * Pi32) {
    //         g_player_angle -= 2 * Pi32;
    //     }
    // }
}





// typedef struct {
//     f32 x;
//     f32 y;
// } v2_t;


// typedef struct {
//     u32 *tiles;
// } tile_chunk_t;


// typedef struct {
//     u32 chunk_shift;
//     u32 chunk_mask;
//     u32 chunk_dim;

//     f32 tile_side_in_meters;

//     u32 tile_chunk_count_x;
//     u32 tile_chunk_count_y;
//     u32 tile_chunk_count_z;

//     tile_chunk_t *tile_chunks;
// } tile_map_t;


// typedef struct {
//     u32 abs_tile_x;
//     u32 abs_tile_y;
//     u32 abs_tile_z;
//     v2_t offset; // NOTE(max): tile relative
// } tile_map_position_t;


// typedef struct {
//     mem_index size;
//     u8 *base;
//     mem_index used;
// } memory_map_t;


// typedef struct {
//     tile_map_t *tile_map;
//     i32 tile_side_in_pixels;
// } world_t;





void draw_rays_2d() {
    // Top
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(526,  0);
    glVertex2i(1006,  0);
    glVertex2i(1006, 160);
    glVertex2i(526, 160);
    glEnd();
    // Bottom
    glColor3f(0, 0, 1);
    glBegin(GL_QUADS);
    glVertex2i(526, 160);
    glVertex2i(1006, 160);
    glVertex2i(1006, 320);
    glVertex2i(526, 320);
    glEnd();

    int r, mx, my, mp, dof;
    float rx, ry, ra, xo, yo, dist_t;

    ra = pa - DR * 30;

    if (ra < 0) {
        ra += 2 * PI;
    }

    if (ra > 2 * PI) {
        ra -= 2 * PI;
    }

    for (r = 0; r < 60; r++) {
        // ----------- Horizontal lines -----------
        dof = 0;
        float a_tan =  -1 / tan(ra);
        float dist_h = 1000000;
        float hx = px;
        float hy = py;

        if (ra > PI) {
            // Looking down
            ry = (((int)py / 64) * 64) - 0.0001;
            rx = (py - ry) * a_tan + px;
            yo = -64;
            xo = -yo * a_tan;
        }

        if (ra < PI) {
            // Looking up
            ry = (((int)py / 64) * 64) + 64;
            rx = (py - ry) * a_tan + px;
            yo = 64;
            xo = -yo * a_tan;
        }

        if (ra == 0 || ra == PI) {
            // Looking straight left or right
            rx = px;
            ry = py;
            dof = 8;
        }

        while (dof < 8) {
            mx = (int) rx / 64;
            my = (int) ry / 64;
            mp = my * map_x + mx;

            if (mp > 0 && mp < map_x * map_y && map[mp] == 1) {
                // Hit wall
                hx = rx;
                hy = ry;
                dist_h = dist(px, py, hx, hy, ra);
                // dist_h = dist(hx - px, hy - py, ra);
                dof = 8;
            } else {
                // Next line
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }

        // Draw
        // glColor3f(0, 1, 0);
        // glLineWidth(9);
        // glBegin(GL_LINES);
        // glVertex2i(px, py);
        // glVertex2i(rx, ry);
        // glEnd();

        // -----------  Vertical lines -----------
        dof = 0;
        float n_tan =  - tan(ra);
        float dist_v = 1000000;
        float vx = px;
        float vy = py;

        if (ra > P2 && ra < P3) {
            // Looking left
            rx = (((int)px / 64) * 64) - 0.0001;
            ry = (px - rx) * n_tan + py;
            xo = -64;
            yo = -xo * n_tan;
        }

        if (ra < P2 || ra > P3) {
            // Looking right
            rx = (((int)px / 64) * 64) + 64;
            ry = (px - rx) * n_tan + py;
            xo = 64;
            yo = -xo * n_tan;
        }

        if (ra == 0 || ra == PI) {
            // Looking straight up or down
            rx = px;
            ry = py;
            dof = 8;
        }

        while (dof < 8) {
            mx = (int) rx / 64;
            my = (int) ry / 64;
            mp = my * map_x + mx;

            if (mp > 0 && mp < map_x * map_y && map[mp] == 1) {
                // Hit wall
                vx = rx;
                vy = ry;
                dist_v = dist(px, py, vx, vy, ra);
                // dist_v = dist(vx - px, vy - py, ra);
                dof = 8;
            } else {
                // Next line
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }

        // ----------- DRAW -----------

        // Set the ray to the shorter
        if (dist_v < dist_h) {
            rx = vx;
            ry = vy;
            dist_t = dist_v;
            glColor3f(0.9, 0, 0);
        }

        if (dist_h < dist_v) {
            rx = hx;
            ry = hy;
            dist_t = dist_h;
            glColor3f(0.6, 0, 0);
        }

        // // Draw 2D
        // glLineWidth(1);
        // glBegin(GL_LINES);
        // glVertex2i(px, py);
        // glVertex2i(rx, ry);
        // glEnd();

        // Draw 3D
        float ca = pa - ra;

        if (ca < 0) {
            ca += 2 * PI;
        }

        if (ca > 2 * PI) {
            ca -= 2 * PI;
        }

        dist_t = dist_t *cos(ca);                       // Fix fisheye

        float line_h = (tile_size * 320) / dist_t;      // Line height

        if (line_h > 320) {
            line_h = 320;
        }

        float line_o = 160 - line_h / 2;                // Line offset

        // Have to split in 2 because on my machine the max line width is less then 8
        glLineWidth(4);
        glBegin(GL_LINES);
        int l = r * 8 + 530 - 2;
        glVertex2i(l, line_o);
        glVertex2i(l, line_h + line_o);
        glEnd();

        glLineWidth(4);
        glBegin(GL_LINES);
        glVertex2i(l + 4, line_o);
        glVertex2i(l + 4, line_h + line_o);
        glEnd();

        // Update the ray angle
        ra += DR;

        if (ra < 0) {
            ra += 2 * PI;
        }

        if (ra > 2 * PI) {
            ra -= 2 * PI;
        }
    }
}