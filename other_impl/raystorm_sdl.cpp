#include <pixello.hpp>
#include <math.h>
#include <vector>
#include <cstdint>
#include <iostream>

#define PI  3.1415926535
#define P2  PI / 2
#define P3  3 * PI/2
#define DR 0.0174533    // one degree in radians

static float px, py;    // player pos
static float pdx, pdy;  // player delta
static float pa;        // player angle


#define texWidth 64
#define texHeight 64
std::vector<uint32_t> texture[8];

static unsigned char *g_pixels = nullptr;
static unsigned int g_width = 0;
static unsigned int g_hight = 0;

// void draw_player() {
//     glColor3f(1, 1, 0);
//     glPointSize(8);
//     glBegin(GL_POINTS);
//     glVertex2i(px, py);
//     glEnd();

//     glLineWidth(3);
//     glBegin(GL_LINES);
//     glVertex2i(px, py);
//     glVertex2i(px + pdx * 5, py + pdy * 5);
//     glEnd();
// }

struct rgba_t {
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;
};


const static int map_x = 8, map_y = 8, tile_size = 64;
int map[] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};


// void draw_map_2d() {
//     int x, y, xo, yo;

//     for (y = 0; y < map_y; y++) {
//         for (x = 0; x < map_x; x++) {

//             if (map[y * map_x + x] == 1) {
//                 //
//                 glColor3f(1.0f, 1.0f, 1.0f);
//             } else {
//                 //
//                 glColor3f(.0f, .0f, .0f);
//             }

//             xo = x * tile_size;
//             yo = y * tile_size;
//             glBegin(GL_QUADS);

//             glVertex2i(xo             + 1, yo + 1);
//             glVertex2i(xo             + 1, yo + tile_size - 1);
//             glVertex2i(xo + tile_size - 1, yo + tile_size - 1);
//             glVertex2i(xo + tile_size - 1, yo + 1);

//             glEnd();
//         }
//     }
// }


// float dist(float dx, float dy, float angle) {
//     return dx * cos(angle) + dy * sin(angle);
// }

float dist(float ax, float ay, float bx, float by, float angle) {
    return (bx - ax) * cos(angle) + (by - ay) * sin(angle);
}

// float dist(float ax, float ay, float bx, float by, float angle) {
//     return sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
// }


void draw_rect(const size_t width,
               const size_t x_pos,
               const size_t top_y,
               const size_t bottom_y,
               rgba_t rgba) {

    for (size_t y = top_y; y < bottom_y; y++) {
        for (size_t x = x_pos; x < x_pos + width; x++) {
            const size_t pixel = (g_width * y + x) * 4;

            g_pixels[pixel + 0] = rgba.B;
            g_pixels[pixel + 1] = rgba.G;
            g_pixels[pixel + 2] = rgba.R;
            g_pixels[pixel + 3] = rgba.A;
        }
    }
}


void draw_top() {
    rgba_t rgb = { 0, 0, 255, 0};
    draw_rect(g_width, 0, 0, g_hight / 2, rgb);
}


void draw_bottom() {
    rgba_t rgb = { 45, 45, 45, 0};
    draw_rect(g_width, 0, g_hight / 2, g_hight, rgb);
}


void draw_rays_2d() {
    // Top
    // glColor3f(0, 0, 0);
    // glBegin(GL_QUADS);
    // glVertex2i(526,  0);
    // glVertex2i(1006,  0);
    // glVertex2i(1006, 160);
    // glVertex2i(526, 160);
    // glEnd();
    draw_top();
    // Bottom
    // glColor3f(0, 0, 1);
    // glBegin(GL_QUADS);
    // glVertex2i(526, 160);
    // glVertex2i(1006, 160);
    // glVertex2i(1006, 320);
    // glVertex2i(526, 320);
    // glEnd();
    draw_bottom();

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
        rgba_t rgb;

        // Set the ray to the shorter
        if (dist_v < dist_h) {
            rx = vx;
            ry = vy;
            dist_t = dist_v;
            // glColor3f(1, 0, 0);
            rgb = {255, 0, 0, 0};
        }

        if (dist_h < dist_v) {
            rx = hx;
            ry = hy;
            dist_t = dist_h;
            // glColor3f(0.6, 0, 0);
            rgb = {153, 0, 0, 0};
        }

        // Draw 2D
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

        // pixels, w, h, width, x_pos, top, bottom
        draw_rect(8, r * 8, line_o, line_h + line_o, rgb);

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


void display() {

    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw_map_2d();
    draw_rays_2d();
    // draw_player();
    // glutSwapBuffers();
}


// void buttons(unsigned char key, int x, int y) {
//     switch (key) {
//     case 'w':
//         px += pdx;
//         py += pdy;
//         break;

//     case 's':
//         px -= pdx;
//         py -= pdy;
//         break;

//     case 'a':
//         pa -= 0.1;

//         if (pa < 0) {
//             pa += 2 * PI;
//         }

//         pdx = cos(pa) * 5;
//         pdy = sin(pa) * 5;

//         break;

//     case 'd':
//         pa += 0.1;

//         if (pa > 2 * PI) {
//             pa -= 2 * PI;
//         }

//         pdx = cos(pa) * 5;
//         pdy = sin(pa) * 5;

//         break;

//     default:
//         break;
//     }

//     glutPostRedisplay();
// }



void init_all() {
    // glClearColor(0.3, 0.3, 0.3, 0);
    // gluOrtho2D(0, 1024, 512, 0);
    px = py = 300;
    pdx = cos(pa) * 5;
    pdy = sin(pa) * 5;


    for (int i = 0; i < 8; i++) {
        texture[i].resize(texWidth * texHeight);
    }

    // Init Textures
    for (int x = 0; x < texWidth; x++) {
        for (int y = 0; y < texHeight; y++) {
            int xorcolor = (x * 256 / texWidth) ^ (y * 256 / texHeight);
            //int xcolor = x * 256 / texWidth;
            int ycolor = y * 256 / texHeight;
            int xycolor = y * 128 / texHeight + x * 128 / texWidth;
            texture[0][texWidth * y + x] = 65536 * 254 * (x != y
                                           && x != texWidth - y); //flat red texture with black cross
            texture[1][texWidth * y + x] = xycolor + 256 * xycolor + 65536 * xycolor; //sloped greyscale
            texture[2][texWidth * y + x] = 256 * xycolor + 65536 * xycolor; //sloped yellow gradient
            texture[3][texWidth * y + x] = xorcolor + 256 * xorcolor + 65536 * xorcolor; //xor greyscale
            texture[4][texWidth * y + x] = 256 * xorcolor; //xor green
            texture[5][texWidth * y + x] = 65536 * 192 * (x % 16 && y % 16); //red bricks
            texture[6][texWidth * y + x] = 65536 * ycolor; //red gradient
            texture[7][texWidth * y + x] = 128 + 256 * 128 + 65536 * 128; //flat grey texture
        }
    }
}


void update(unsigned char *pixels,
            const unsigned int w,
            const unsigned int h,
            const unsigned int c) {

    g_pixels = pixels;
    g_width = w;
    g_hight = h;

    display();
}


void key_function(const unsigned int type, const unsigned int state, const unsigned int key_code) {
    if (type == 768) {
        switch (key_code) {
        case 'w':
            px += pdx;
            py += pdy;
            break;

        case 's':
            px -= pdx;
            py -= pdy;
            break;

        case 'a':
            pa -= 0.1;

            if (pa < 0) {
                pa += 2 * PI;
            }

            pdx = cos(pa) * 5;
            pdy = sin(pa) * 5;

            break;

        case 'd':
            pa += 0.1;

            if (pa > 2 * PI) {
                pa -= 2 * PI;
            }

            pdx = cos(pa) * 5;
            pdy = sin(pa) * 5;

            break;

        default:
            break;
        }
    }
}



// MAIN
int main(int argc, char **argv) {

    if (!init()) {
        return 1;
    }

    if (!create_window("Pixello Test", 480, 320)) {
        return 2;
    }

    init_all();

    if (!run(&update, &key_function, 1)) {
        return 3;
    }

    close();

    return 0;
}