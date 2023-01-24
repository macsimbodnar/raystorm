#pragma once
#include <cstdint>

#define PI (3.14159265359f)
#define P2 (PI / 2.0f)
#define P3 (3 * PI / 2.0f)
#define ONC_DEGREE_IN_RADIANS (0.0174533f)
#define TAU (2.0f * PI)
#define TO_F32(_X_) static_cast<float>(_X_)
#define TO_INT(_X_) static_cast<int>(_X_)

int32_t round_f32_to_i32(float r32);
uint32_t round_f32_to_u32(float r32);
int32_t floor_f32_to_i32(float r32);
int32_t ceil_f32_to_i32(float r32);
float floor_f32(float r32);
float ceil_f32(float r32);
float abs_f32(float i);
float tan_f32(float x);
float atan2_f32(float y, float x);
float hypot_f32(float x, float y);
int32_t abs_i32(int32_t i);
float remainder_f32(float numer, float denom);

float sin_f32(float x);
float cos_f32(float x);
float squaref(float x);
float pow_of_two_f32(float a);

float mod_f32(float r32);

float dist_f32(float ax, float ay, float bx, float by, float angle);
