#include "intrinsics.h"
#include "math.h"
#include "stdlib.h"  // For abs

i32 round_f32_to_i32(f32 r32)
{
  i32 result = (i32)roundf(r32);
  return result;
}

u32 round_f32_to_u32(f32 r32)
{
  u32 result = (u32)roundf(r32);
  return result;
}

i32 floor_f32_to_i32(f32 r32)
{
  i32 result = (i32)floorf(r32);
  return result;
}

i32 ceil_f32_to_i32(f32 r32)
{
  i32 result = (i32)ceilf(r32);
  return result;
}

f32 floor_f32(f32 r32)
{
  f32 result = floorf(r32);
  return result;
}

f32 ceil_f32(f32 r32)
{
  f32 result = ceilf(r32);
  return result;
}

f32 abs_f32(f32 i)
{
  f32 result = (f32)fabs(i);
  return result;
}

i32 abs_i32(i32 i)
{
  i32 result = (i32)abs(i);
  return result;
}

f32 sin_f32(f32 x)
{
  f32 result = (f32)sin(x);
  return result;
}

f32 cos_f32(f32 x)
{
  f32 result = (f32)cos(x);
  return result;
}

f32 tanf(f32 x)
{
  f32 result = (f32)tan(x);
  return result;
}

f32 squaref(f32 a)
{
  return a * a;
}

f32 mod_f32(f32 r32)
{
  f32 result = r32 - floor_f32(r32);
  assert(result < 1.0f);
  return result;
}
