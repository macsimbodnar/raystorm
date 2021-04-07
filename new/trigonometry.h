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
typedef point_f32_t v2_t;


f32 distf(f32 ax, f32 ay, f32 bx, f32 by, f32 angle);

f32 cos_f32(f32 X);
f32 sin_f32(f32 X);

v2_t scalar_mul_f32(v2_t vec, const f32 scalar);
v2_t vec_sum_f32(v2_t A, v2_t B);
f32 inner_mul_f32(v2_t A, v2_t B);