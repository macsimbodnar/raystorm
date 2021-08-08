#include "world.h"
#include "intrinsics.h"

void update_world(world_t* world, const game_input_t* input)
{
  const game_controller_input_t* controller = &input->controllers[0];

  f32 direction = .0f;

  // TODO(max): Update the angle based on the elapsed time and not by a const
  // amount
  if (controller->up.ended_down) { direction += 1.0f; }

  if (controller->down.ended_down) { direction += -1.0f; }

  if (controller->left.ended_down) {
    world->player.angle_rad += 0.03f;

    if (world->player.angle_rad < 0) { world->player.angle_rad += 2 * PI; }
  }

  if (controller->right.ended_down) {
    world->player.angle_rad -= 0.03f;

    if (world->player.angle_rad > 2 * PI) { world->player.angle_rad -= 2 * PI; }
  }

  const f32 delta_space = 0.005f * direction / input->dt_for_frame;

  f32 player_offset_X = cos_f32(world->player.angle_rad) * delta_space;
  f32 player_offset_Y = sin_f32(world->player.angle_rad) * delta_space;

  // Calculate the new player position based on his velocity and the time
  // elapsed from the last check
  position_t new_player_pos = world->player.pos;
  new_player_pos.X = new_player_pos.X + player_offset_X;
  new_player_pos.Y = new_player_pos.Y + player_offset_Y;

  world->player.pos = new_player_pos;

  // Set the camera to the same postion as the player
  world->camera.pos = world->player.pos;
}