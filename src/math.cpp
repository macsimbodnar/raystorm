#include "math.hpp"
#include <stdlib.h>  // For abs
#include <cassert>
#include <cmath>

#define TO_I32(_X_) static_cast<int32_t>(_X_)
#define TO_U32(_X_) static_cast<uint32_t>(_X_)

int32_t round_f32_to_i32(float r32)
{
  int32_t result = TO_I32(roundf(r32));
  return result;
}

uint32_t round_f32_to_u32(float r32)
{
  uint32_t result = TO_U32(roundf(r32));
  return result;
}

int32_t floor_f32_to_i32(float r32)
{
  int32_t result = TO_I32(floorf(r32));
  return result;
}

int32_t ceil_f32_to_i32(float r32)
{
  int32_t result = TO_I32(ceilf(r32));
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
  float result = TO_F32(fabs(i));
  return result;
}

int32_t abs_i32(int32_t i)
{
  int32_t result = TO_I32(abs(i));
  return result;
}

float sin_f32(float x)
{
  float result = TO_F32(sin(x));
  return result;
}

float cos_f32(float x)
{
  float result = TO_F32(cos(x));
  return result;
}

float tan_f32(float x)
{
  float result = TO_F32(tan(x));
  return result;
}

float atan2_f32(float y, float x)
{
  float result = TO_F32(atan2(y, x));
  return result;
}

float hypot_f32(float x, float y)
{
  float result = hypotf(x, y);
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
