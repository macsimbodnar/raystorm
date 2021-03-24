#include "common_platform.h"

game_controller_input_t *get_controller(game_input_t *input, unsigned int index) {
    ASSERT(index < ARRAY_COUNT(input->controllers));
    return &input->controllers[index];
}


f32 square(f32 a) {
    return a * a;
}