#pragma once
#include "common_platform.h"


#define PI                          3.14159265359f
#define P2                          PIf / 2
#define P3                          3 * PIf / 2
#define ONCE_DEGREE_IN_RADIANS      0.0174533f

typedef struct {
    i32 x;
    i32 y;
    i32 width;
    i32 height;
} rect_t;


typedef struct {
    i32 X;
    i32 Y;
} point_i32_t;


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