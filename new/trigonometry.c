#include "trigonometry.h"
#include "intrinsics.h"


f32 distf(f32 ax, f32 ay, f32 bx, f32 by, f32 angle) {
    return (bx - ax) * cosf(angle) + (by - ay) * sinf(angle);
}
