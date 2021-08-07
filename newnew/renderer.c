#include "renderer.h"
#include "intrinsics.h"

global_var color_t RED = {1.0f, .0f, .0f};
global_var color_t GREEN = {.0f, 1.0f, .0f};
global_var color_t BLUE = {.0f, .0f, 1.0f};
global_var color_t WHITE = {1.0f, 1.0f, 1.0f};
global_var color_t BLACK = {.0f, .0f, .0f};
global_var color_t PURPLE = {1.0f, .0f, 1.0f};
global_var color_t YELLOW = {1.0f, 1.0f, .0f};

internal void draw_line(game_offscreen_buffer_t* buffer,
                        point_i32_t A,
                        point_i32_t B,
                        color_t color)
{
  // Use Bresenham's principles of integer incremental error to perform all
  // octant line draws

  // *INDENT-OFF*
  if (A.X > buffer->width)
    A.X = buffer->width;
  if (A.X < 0)
    A.X = 0;
  if (A.Y > buffer->height)
    A.Y = buffer->height;
  if (A.Y < 0)
    A.Y = 0;
  if (B.X > buffer->width)
    B.X = buffer->width;
  if (B.X < 0)
    B.X = 0;
  if (B.Y > buffer->height)
    B.Y = buffer->height;
  if (B.Y < 0)
    B.Y = 0;
  // *INDENT-ON*

  u32* pixel;
  u32 color_32 = ((round_f32_to_u32(color.R * 255.0f) << 16) |
                  (round_f32_to_u32(color.G * 255.0f) << 8) |
                  (round_f32_to_u32(color.B * 255.0f) << 0));

  i32 dx = abs_i32(A.X - B.X);
  i32 sx = B.X < A.X ? 1 : -1;
  i32 dy = -abs_i32(A.Y - B.Y);
  i32 sy = B.Y < A.Y ? 1 : -1;
  i32 err = dx + dy;
  i32 x = B.X;
  i32 y = B.Y;
  i32 e2;

  while (true) {
    pixel = (u32*)(((u8*)buffer->memory) + x * buffer->bytes_per_pixel +
                   y * buffer->pitch);
    *pixel = color_32;

    if (x == A.X && y == A.Y) {
      break;
    }

    e2 = 2 * err;

    if (e2 >= dy) {
      err += dy;
      x += sx;
    }

    if (e2 <= dx) {
      err += dx;
      y += sy;
    }
  }
}

internal void draw_rectangle(game_offscreen_buffer_t* buffer,
                             rect_t rect,
                             color_t color)
{
  i32 min_x = rect.x;
  i32 min_y = rect.y;
  i32 max_x = rect.x + rect.width;
  i32 max_y = rect.y + rect.height;

  // *INDENT-OFF*
  if (min_x < 0)
    min_x = 0;
  if (min_y < 0)
    min_y = 0;
  if (max_x > buffer->width)
    max_x = buffer->width;
  if (max_y > buffer->height)
    max_y = buffer->height;
  // *INDENT-ON*

  u32 color_32 = ((round_f32_to_u32(color.R * 255.0f) << 16) |
                  (round_f32_to_u32(color.G * 255.0f) << 8) |
                  (round_f32_to_u32(color.B * 255.0f) << 0));

  u8* row = (((u8*)buffer->memory) + min_x * buffer->bytes_per_pixel +
             min_y * buffer->pitch);

  for (int y = min_y; y < max_y; y++) {
    u32* pixel = (u32*)row;

    for (int x = min_x; x < max_x; x++) {
      *pixel++ = color_32;
    }

    row += buffer->pitch;
  }
}

internal void draw_buffer(game_offscreen_buffer_t* dest_buffer,
                          i32 X,
                          i32 Y,
                          const game_offscreen_buffer_t* source_buffer)
{
  i32 min_x = X;
  i32 min_y = Y;
  i32 max_x = min_x + source_buffer->width;
  i32 max_y = min_y + source_buffer->height;

  // *INDENT-OFF*
  if (min_x < 0)
    min_x = 0;
  if (min_y < 0)
    min_y = 0;
  if (max_x > dest_buffer->width)
    max_x = dest_buffer->width;
  if (max_y > dest_buffer->height)
    max_y = dest_buffer->height;
  // *INDENT-ON*

  u8* row = (((u8*)dest_buffer->memory) + min_x * dest_buffer->bytes_per_pixel +
             min_y * dest_buffer->pitch);
  u8* source_row = source_buffer->memory;

  for (int y = min_y; y < max_y; y++) {
    u32* pixel = (u32*)row;
    u32* source_pixel = (u32*)source_row;

    for (int x = min_x; x < max_x; x++) {
      *pixel = *source_pixel;
      ++pixel;
      ++source_pixel;
    }

    row += dest_buffer->pitch;
    source_row += source_buffer->pitch;
  }
}

u8 get_tile_value_in_raw_coordinates(const tiles_t* chunk, u32 X, u32 Y)
{
  u8 res = chunk->tiles[Y * chunk->W + X];
  return res;
}

position_t world_to_real_pos(const tiles_t* chunk, position_t pos)
{
  position_t res;
  res.X = pos.X;

  // Invert the the Y axis
  res.Y = chunk->H - pos.Y;

  return res;
}

void draw_world(game_offscreen_buffer_t* buffer, const world_t* world) {}

void draw_minimap(game_offscreen_buffer_t* screen_buffer, const world_t* world)
{
  game_offscreen_buffer_t buffer;
  buffer.bytes_per_pixel = screen_buffer->bytes_per_pixel;
  buffer.height = 150;
  buffer.width = 300;
  buffer.pitch = buffer.width * buffer.bytes_per_pixel;
  char mem[4 * 150 * 300];  // = buffer->pitch * buffer->height
  buffer.memory = (void*)&mem;

  rect_t screen = {0, 0, buffer.width, buffer.height};
  color_t screen_color = {0.5f, 0.5f, 0.5f};
  draw_rectangle(&buffer, screen, screen_color);

  // DRAW MAP

  const tiles_t* chunk = world->map.current_chunk;
  i32 half_buffer_w = buffer.width / 2;
  i32 half_buffer_h = buffer.height / 2;
  f32 meters_to_pixels = world->meter_to_pixel_multiplier;
  position_t camera = world_to_real_pos(chunk, world->camera.pos);

  // Avoid to loop on tails that are out of the screen
  i32 min_x = floor_f32_to_i32(camera.X - (half_buffer_w / meters_to_pixels));

  if (min_x < 0) {
    min_x = 0;
  }

  i32 min_y = floor_f32_to_i32(camera.Y - (half_buffer_h / meters_to_pixels));

  if (min_y < 0) {
    min_y = 0;
  }

  i32 max_x = ceil_f32_to_i32(camera.X + (half_buffer_w / meters_to_pixels));

  if (max_x > chunk->W) {
    max_x = chunk->W;
  }

  i32 max_y = ceil_f32_to_i32(camera.Y + (half_buffer_h / meters_to_pixels));

  if (max_y > chunk->H) {
    max_y = chunk->H;
  }

  color_t color = {};
  rect_t tile = {};
  tile.height = chunk->side_in_meters * meters_to_pixels - 1;
  tile.width = chunk->side_in_meters * meters_to_pixels - 1;

  for (i32 y = min_y; y < max_y; ++y) {
    for (i32 x = min_x; x < max_x; ++x) {
      u8 val = get_tile_value_in_raw_coordinates(chunk, x, y);

      if (val) {
        color = (color_t){0.8f, 0.8f, 0.8f};
      } else {
        color = (color_t){0.1f, 0.1f, 0.1f};
      }

      tile.x = 1 + (half_buffer_w +
                    round_f32_to_i32((x - camera.X) * meters_to_pixels));
      tile.y = 1 + (half_buffer_h +
                    round_f32_to_i32((y - camera.Y) * meters_to_pixels));

      draw_rectangle(&buffer, tile, color);
    }
  }

  // DRAW PLAYER
  i32 player_pixels_w = round_f32_to_i32(world->player.W * meters_to_pixels);
  i32 player_pixels_h = round_f32_to_i32(world->player.H * meters_to_pixels);
  i32 player_pixels_w_half = round_f32_to_i32(player_pixels_w / 2);
  i32 player_pixels_h_half = round_f32_to_i32(player_pixels_h / 2);

  position_t player_tile;
  player_tile.X = world->player.pos.X / chunk->side_in_meters;
  player_tile.Y = world->player.pos.Y / chunk->side_in_meters;

  player_tile = world_to_real_pos(chunk, player_tile);

  // Draw player tiless
  rect_t player_tile_rect = {
      half_buffer_w +
          (floor_f32_to_i32(player_tile.X) - camera.X) * meters_to_pixels,
      half_buffer_h +
          (floor_f32_to_i32(player_tile.Y) - camera.Y) * meters_to_pixels,
      chunk->side_in_meters * meters_to_pixels,
      chunk->side_in_meters * meters_to_pixels};

  draw_rectangle(&buffer, player_tile_rect, YELLOW);

  // Player rect
  i32 px = (buffer.width / 2) - player_pixels_w_half;
  i32 py = (buffer.height / 2) - player_pixels_h_half;
  rect_t player_rect = {px, py, player_pixels_w, player_pixels_h};

  // Draw player pos
  draw_rectangle(&buffer, player_rect, RED);

  // Draw the player direction ray
  f32 pdx = cos_f32(world->player.angle);
  f32 pdy = -sin_f32(world->player.angle);

  point_i32_t start_point = {player_rect.x + player_pixels_w_half,
                             player_rect.y + player_pixels_h_half};
  point_i32_t end_point;
  end_point.X = start_point.X + round_f32_to_i32(pdx * 30);
  end_point.Y = start_point.Y + round_f32_to_i32(pdy * 30);

  draw_line(&buffer, start_point, end_point, GREEN);

  // Draw the minimap buffer into the main buffer
  draw_buffer(screen_buffer, 0, screen_buffer->height - buffer.height, &buffer);
}