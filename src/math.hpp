#pragma once
#include <cstdint>

#define PI (3.14159265359f)
#define P2 (PIf / 2)
#define P3 (3 * PIf / 2)
#define ONC_DEGREE_IN_RADIANS (0.0174533f)
#define TAU (2 * PI)

int32_t round_f32_to_i32(float r32);
uint32_t round_f32_to_u32(float r32);
int32_t floor_f32_to_i32(float r32);
int32_t ceil_f32_to_i32(float r32);
float floor_f32(float r32);
float ceil_f32(float r32);
float abs_f32(float i);
int32_t abs_i32(int32_t i);
float remainder_f32(float numer, float denom);

float sin_f32(float x);
float cos_f32(float x);
float squaref(float x);

float squaref(float a);

float mod_f32(float r32);
