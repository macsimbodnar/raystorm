#pragma once
#include "common_platform.h"


#define PI                          3.14159265359f
#define P2                          PIf / 2
#define P3                          3 * PIf / 2
#define ONCE_DEGREE_IN_RADIANS      0.0174533f

typedef struct {
    u32 x;
    u32 y;
    u32 width;
    u32 height;
} rect_t;


typedef struct {
    u32 X;
    u32 Y;
} point_u32_t;


typedef struct {
    f32 X;
    f32 Y;
} point_f32_t;


// Vector with that starts from origin and point to the x,y
typedef struct {
    f32 x;
    f32 y;
} v2_t;


f32 distf(f32 ax, f32 ay, f32 bx, f32 by, f32 angle);