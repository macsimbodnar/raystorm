#include "intrinsics.h"
#include "math.h"


u32 round_f32_to_u32(f32 r32) {
    u32 result = (u32) roundf(r32);
    return result;
}

f32 sinf(f32 x) {
    f32 result = (f32) sin(x);
    return result;
}

f32 cosf(f32 x) {
    f32 result = (f32) cos(x);
    return result;
}

f32 tanf(f32 x) {
    f32 result = (f32) tan(x);
    return result;
}

f32 squaref(f32 a) {
    return a * a;
}