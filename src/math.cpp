#include "math.hpp"
#include <stdlib.h>  // For abs
#include <cassert>
#include <cmath>

int32_t round_f32_to_i32(float r32)
{
  int32_t result = (int32_t)roundf(r32);
  return result;
}

uint32_t round_f32_to_u32(float r32)
{
  uint32_t result = (uint32_t)roundf(r32);
  return result;
}

int32_t floor_f32_to_i32(float r32)
{
  int32_t result = (int32_t)floorf(r32);
  return result;
}

int32_t ceil_f32_to_i32(float r32)
{
  int32_t result = (int32_t)ceilf(r32);
  return result;
}

float floor_f32(float r32)
{
  float result = floorf(r32);
  return result;
}

float ceil_f32(float r32)
{
  float result = ceilf(r32);
  return result;
}

float abs_f32(float i)
{
  float result = (float)fabs(i);
  return result;
}

int32_t abs_i32(int32_t i)
{
  int32_t result = (int32_t)abs(i);
  return result;
}

float sin_f32(float x)
{
  float result = (float)sin(x);
  return result;
}

float cos_f32(float x)
{
  float result = (float)cos(x);
  return result;
}

float tan_f32(float x)
{
  float result = (float)tan(x);
  return result;
}

float squaref(float a)
{
  return a * a;
}

float mod_f32(float r32)
{
  float result = r32 - floor_f32(r32);
  assert(result < 1.0f);
  return result;
}


float remainder_f32(float numer, float denom)
{
  float result = remainder(numer, denom);
  return result;
}


float pow_of_two_f32(float a)
{
  return a * a;
}


float dist_f32(float ax, float ay, float bx, float by, float angle)
{
  return (bx - ax) * cos(angle) + (by - ay) * sin(angle);
}
