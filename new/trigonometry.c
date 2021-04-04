#include "trigonometry.h"
#include "intrinsics.h"


f32 distf(f32 ax, f32 ay, f32 bx, f32 by, f32 angle) {
    return (bx - ax) * cosf(angle) + (by - ay) * sinf(angle);
}

f32 cos_f32(f32 X) {
    f32 result = cosf(X);
    return result;
}

f32 sin_f32(f32 X) {
    f32 result = sinf(X);
    return result;
}
