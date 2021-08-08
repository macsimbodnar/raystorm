#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <iostream>

// clang-format off
// Textures 32 x 32
int all_textures [] = {
    // Checkerboard
    0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,0,0,1,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,0,1,1,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,1,1,1,1,1,1,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,1,1,1,1,1,1,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,0,1,1,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,0,0,1,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,

    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,

    0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 
    0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,

    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,

    // Brick
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,

    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,

    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,

    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,

    // Window
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,

    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,

    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,

    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,

    // Door
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,1,1,1,1, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 1,1,1,1,0,0,0,0,
    0,0,0,0,1,0,0,1, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 1,0,0,1,0,0,0,0,
    0,0,0,0,1,0,0,1, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 1,0,0,1,0,0,0,0,
    0,0,0,0,1,0,0,1, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 1,0,0,1,0,0,0,0,

    0,0,0,0,1,0,0,1, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 1,0,0,1,0,0,0,0,
    0,0,0,0,1,0,0,1, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 1,0,0,1,0,0,0,0,
    0,0,0,0,1,0,0,1, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 1,0,0,1,0,0,0,0,
    0,0,0,0,1,0,0,1, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 1,0,0,1,0,0,0,0,
    0,0,0,0,1,0,0,1, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 1,0,0,1,0,0,0,0,
    0,0,0,0,1,0,0,1, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 1,0,0,1,0,0,0,0,
    0,0,0,0,1,0,0,1, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 1,0,0,1,0,0,0,0,
    0,0,0,0,1,1,1,1, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 1,1,1,1,0,0,0,0,

    0,0,0,0,0,0,0,0, 0,0,0,0,0,1,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,1,1,1,1,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,

    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0,
};
// clang-format on

#define PI 3.1415926535f
#define P2 PI / 2
#define P3 3 * PI / 2
#define DR 0.0174533f  // one degree in radians

struct button_keys_t
{
  int w;
  int a;
  int d;
  int s;
  int e;
};

static float px, py;    // player pos
static float pdx, pdy;  // player delta
static float pa;        // player angle
static button_keys_t global_keys;
static float frame_1;
static float frame_2;
static float fps;

void draw_player()
{
  glColor3f(1, 1, 0);
  glPointSize(8);
  glBegin(GL_POINTS);
  glVertex2i(px, py);
  glEnd();

  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex2i(px, py);
  glVertex2i(px + pdx * 5, py + pdy * 5);
  glEnd();
}

const static int map_x = 8, map_y = 8, tile_size = 64;

// clang-format off
int mapw[] = {                  // Map of walls
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 0, 0, 0, 2, 0, 0, 2,
    2, 0, 0, 0, 2, 0, 0, 2,
    2, 2, 4, 2, 2, 0, 0, 3,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 1, 2, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 2, 2, 2, 2, 2, 2, 2
};

int mapf[] = {                  // Map of floors
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

int mapc[] = {                  // Map of ceiling
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
// clang-format on

void draw_map_2d()
{
  int x, y, xo, yo;

  for (y = 0; y < map_y; y++) {
    for (x = 0; x < map_x; x++) {
      if (mapw[y * map_x + x] > 0) {
        //
        glColor3f(1.0f, 1.0f, 1.0f);
      } else {
        //
        glColor3f(.0f, .0f, .0f);
      }

      xo = x * tile_size;
      yo = y * tile_size;
      glBegin(GL_QUADS);

      glVertex2i(xo + 1, yo + 1);
      glVertex2i(xo + 1, yo + tile_size - 1);
      glVertex2i(xo + tile_size - 1, yo + tile_size - 1);
      glVertex2i(xo + tile_size - 1, yo + 1);

      glEnd();
    }
  }
}

// float dist(float dx, float dy, float angle) {
//     return dx * cos(angle) + dy * sin(angle);
// }

float dist(float ax, float ay, float bx, float by, float angle)
{
  return (bx - ax) * cos(angle) + (by - ay) * sin(angle);
}

// float dist(float ax, float ay, float bx, float by, float angle) {
//     return sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
// }

void draw_rays_2d()
{
  // Top
  glColor3f(0.5, 0.1, .0);
  glBegin(GL_QUADS);
  glVertex2i(526, 0);
  glVertex2i(1006, 0);
  glVertex2i(1006, 160);
  glVertex2i(526, 160);
  glEnd();
  // Bottom
  glColor3f(0.8, 0.6, 0.4);
  glBegin(GL_QUADS);
  glVertex2i(526, 160);
  glVertex2i(1006, 160);
  glVertex2i(1006, 320);
  glVertex2i(526, 320);
  glEnd();

  int r, mx, my, mp, dof;
  float rx, ry, ra, xo, yo, dist_t;

  ra = pa - DR * 30;

  if (ra < 0) { ra += 2 * PI; }

  if (ra > 2 * PI) { ra -= 2 * PI; }

  for (r = 0; r < 60; r++) {
    int vmt = 0;  // Vertival mapw texture
    int hmt = 0;  // Horizontal mapw texture

    // ----------- Horizontal lines -----------
    dof = 0;
    float a_tan = -1 / tan(ra);
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
      mx = (int)rx / 64;
      my = (int)ry / 64;
      mp = my * map_x + mx;

      if (mp > 0 && mp < map_x * map_y && mapw[mp] > 0) {
        // Hit wall
        hmt = mapw[mp] - 1;
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
    float n_tan = -tan(ra);
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
      mx = (int)rx / 64;
      my = (int)ry / 64;
      mp = my * map_x + mx;

      if (mp > 0 && mp < map_x * map_y && mapw[mp] > 0) {
        // Hit wall
        vmt = mapw[mp] - 1;
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

    float shade = 1.0f;
    const float ray_r_color = 1.0f;

    // Set the ray to the shorter
    if (dist_v < dist_h) {
      hmt = vmt;
      rx = vx;
      ry = vy;
      dist_t = dist_v;
      glColor3f(ray_r_color, 0, 0);
    }

    if (dist_h < dist_v) {
      shade = 0.5f;
      rx = hx;
      ry = hy;
      dist_t = dist_h;
      glColor3f(ray_r_color * shade, 0, 0);
    }

    // Draw 2D
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2i(px, py);
    glVertex2i(rx, ry);
    glEnd();

    // Draw 3D
    float ca = pa - ra;

    if (ca < 0) { ca += 2 * PI; }

    if (ca > 2 * PI) { ca -= 2 * PI; }

    dist_t = dist_t * cos(ca);  // Fix fisheye

    float line_h = (tile_size * 320) / dist_t;  // Line height

    float ty_step = 32.0 / (float)line_h;
    float ty_off = 0;

    if (line_h > 320) {
      ty_off = (line_h - 320) / 2.0f;
      line_h = 320;
    }

    float line_off = 160 - line_h / 2;  // Line offset

    float ty = ty_off * ty_step + hmt * 32;  // Texture Y value
    float tx;                                // Texture X value

    if (shade == 1.0f) {  // Draw the textures in the wall in front and backward
      tx = (int)(ry / 2.0f) % 32;

      if (ra > P2 && ra < P3) { tx = 31 - tx; }
    } else {  // Draw the textures in the left and right wall
      tx = (int)(rx / 2.0f) % 32;

      if (ra < PI) {
        // Flip the texture if we looking backward
        tx = 31 - tx;
      }
    }

    for (int y = 0; y < line_h; ++y) {
      // DRAW WALLS

      // Have to split in 2 because on my machine the max line width is less
      // then 8 glLineWidth(4); glBegin(GL_LINES); int l = r * 8 + 530 - 2;
      // glVertex2i(l, line_off);
      // glVertex2i(l, line_h + line_off);
      // glEnd();

      // glLineWidth(4);
      // glBegin(GL_LINES);
      // glVertex2i(l + 4, line_off);
      // glVertex2i(l + 4, line_h + line_off);
      // glEnd();

      float c = all_textures[((int)(ty)*32) + (int)tx] * shade;

      // Set different colors for different walls
      if (hmt == 0) {
        glColor3f(c, c / 2.0, c / 2.0);  // Checkerboard red
      } else if (hmt == 1) {
        glColor3f(c, c, c / 2.0);  // Brick yellow
      } else if (hmt == 2) {
        glColor3f(c / 2.0, c / 2.0, c);  // Window blue
      } else if (hmt == 3) {
        glColor3f(c / 2.0, c, c / 2.0);  // Door green
      }

      glPointSize(8);
      glBegin(GL_POINTS);
      glVertex2i(r * 8 + 530, y + line_off);
      glEnd();
      ty += ty_step;
    }

    for (int y = line_off + line_h; y < 320; ++y) {
      // Draw floor
      float dy = y - (320 / 2.0);
      float angle = pa - ra;

      if (angle < 0) { angle += 2 * PI; }

      if (angle > 2 * PI) { angle -= 2 * PI; }

      float ra_fix = cos(angle);

      float tx = px / 2 + cos(ra) * 158 * 32 / dy / ra_fix;
      float ty = py / 2 + sin(ra) * 158 * 32 / dy / ra_fix;

      int mp = mapf[(int)(ty / 32.0) * map_x + (int)(tx / 32.0)] * 32 * 32;
      float c = all_textures[((int)(ty)&31) * 32 + ((int)(tx)&31) + mp] * 0.7;

      glColor3f(c / 1.3, c / 1.3, c);
      glPointSize(8);
      glBegin(GL_POINTS);
      glVertex2i(r * 8 + 530, y);
      glEnd();

      // Draw ceiling
      mp = mapc[(int)(ty / 32.0) * map_x + (int)(tx / 32.0)] * 32 * 32;
      c = all_textures[((int)(ty)&31) * 32 + ((int)(tx)&31) + mp] * 0.7;

      glColor3f(c / 2.0, c / 1.2, c / 2.0);
      glPointSize(8);
      glBegin(GL_POINTS);
      glVertex2i(r * 8 + 530, 320 - y);
      glEnd();
    }

    // Update the ray angle
    ra += DR;

    if (ra < 0) { ra += 2 * PI; }

    if (ra > 2 * PI) { ra -= 2 * PI; }
  }
}

void process_input()
{
  ////////// Collision
  // Offset
  int x0 = 0;

  if (pdx < 0) {
    x0 = -15;
  } else {
    x0 = 15;
  }

  int y0 = 0;

  if (pdy < 0) {
    y0 = -15;
  } else {
    y0 = 15;
  }

  // Calculate the offset postion
  int ipx = px / 64.0f;
  int ipx_add_x0 = (px + x0) / 64.0f;
  int ipx_sub_x0 = (px - x0) / 64.0f;
  int ipy = py / 64.0f;
  int ipy_add_y0 = (py + y0) / 64.0f;
  int ipy_sub_y0 = (py - y0) / 64.0f;

  ////////// Update the pos
  const float delta = 0.2 * fps;

  if (global_keys.w == 1) {
    if (mapw[ipy * map_x + ipx_add_x0] == 0) { px += pdx; }

    if (mapw[ipy_add_y0 * map_x + ipx] == 0) { py += pdy; }
  }

  if (global_keys.s == 1) {
    if (mapw[ipy * map_x + ipx_sub_x0] == 0) { px -= pdx; }

    if (mapw[ipy_sub_y0 * map_x + ipx] == 0) { py -= pdy; }
  }

  if (global_keys.a == 1) {
    pa -= 0.03;

    if (pa < 0) { pa += 2 * PI; }

    pdx = cos(pa) * delta;
    pdy = sin(pa) * delta;
  }

  if (global_keys.d == 1) {
    pa += 0.03;

    if (pa > 2 * PI) { pa -= 2 * PI; }

    pdx = cos(pa) * delta;
    pdy = sin(pa) * delta;
  }

  if (global_keys.e == 1) {
    // Open doors
    int x0 = 0;
    int y0 = 0;

    if (pdx < 0) {
      x0 = -25;
    } else {
      x0 = 25;
    }

    if (pdy < 0) {
      y0 = -25;
    } else {
      y0 = 25;
    }

    int ipx = px / 64.0;
    int ipx_add_x0 = (px + x0) / 64.0;
    int ipy = py / 64.0;
    int ipy_add_y0 = (py + y0) / 64.0;

    int tail = ipy_add_y0 * map_x + ipx_add_x0;

    if (mapw[tail] == 4) { mapw[tail] = 0; }
  }

  glutPostRedisplay();
}

void display()
{
  // Get the fps
  frame_2 = glutGet(GLUT_ELAPSED_TIME);
  fps = frame_2 - frame_1;
  frame_1 = glutGet(GLUT_ELAPSED_TIME);

  // Check the keyboard
  process_input();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw_map_2d();
  draw_rays_2d();
  draw_player();
  glutSwapBuffers();
}

void button_down(const unsigned char key, int x, int y)
{
  switch (key) {
    case 'a':
      global_keys.a = 1;
      break;

    case 'd':
      global_keys.d = 1;
      break;

    case 'w':
      global_keys.w = 1;
      break;

    case 's':
      global_keys.s = 1;
      break;

    case 'e':
      global_keys.e = 1;
      break;
  }

  glutPostRedisplay();
}

void button_up(const unsigned char key, int x, int y)
{
  switch (key) {
    case 'a':
      global_keys.a = 0;
      break;

    case 'd':
      global_keys.d = 0;
      break;

    case 'w':
      global_keys.w = 0;
      break;

    case 's':
      global_keys.s = 0;
      break;

    case 'e':
      global_keys.e = 0;
      break;
  }

  glutPostRedisplay();
}

void resize(const int w, const int h)
{
  glutReshapeWindow(1024, 512);
}

void init()
{
  glClearColor(0.3, 0.3, 0.3, 0);
  gluOrtho2D(0, 1024, 512, 0);
  px = py = 300;
  pdx = cos(pa) * 5;
  pdy = sin(pa) * 5;
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(1024, 512);
  glutInitWindowPosition(300, 1200);
  glutCreateWindow("RAY STORM");

  init();

  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutKeyboardFunc(button_down);
  glutKeyboardUpFunc(button_up);

  glutMainLoop();

  return 0;
}