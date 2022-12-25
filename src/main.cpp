#include <cassert>
#include <pixello.hpp>
#include "log.hpp"
#include "math.hpp"

constexpr int SCREEN_W = 1024;
constexpr int SCREEN_H = 768;
constexpr int TILE = 32;
constexpr float PLAYER_SPEED = 0.0000001;
constexpr float PLAYER_ROTATION_SPEED = 0.000000001;

constexpr float FOW = PI / 3;
constexpr float HALF_FOW = FOW / 2;
constexpr int NUM_RAYS = SCREEN_W / 2;
// constexpr int HALF_NUM_RAYS = NUM_RAYS / 2;
constexpr float DELTA_ANGLE = FOW / static_cast<float>(NUM_RAYS);
// constexpr int MAX_DEPTH = 20;

// clang-format off
constexpr char minimap[SCREEN_H / TILE][SCREEN_W / TILE] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};
// clang-format on

struct player_t
{
  point_t pixel_pos;
  float angle;

  point_t tile_pos() const
  {
    const point_t tile = {pixel_pos.x / TILE, pixel_pos.y / TILE};
    return tile;
  }
};

class gui_t : public pixello
{
private:
  player_t player;

public:
  gui_t()
      : pixello(SCREEN_W,
                SCREEN_H,
                "Raystorm",
                60,
                "assets/font/PressStart2P.ttf",
                10)
  {}

private:
  point_t tile_to_pixel(const point_t& p)
  {
    point_t res = {p.x * TILE, p.y * TILE};
    return res;
  }


  void ray_cast()
  {
    const point_t player_pos = player.pixel_pos;
    const point_t player_tile_pos = player.tile_pos();

    // float ray_angle = player.angle;
    float ray_angle = player.angle - HALF_FOW;
    for (int i = 0; i < NUM_RAYS; ++i, ray_angle += DELTA_ANGLE) {
      ray_angle = remainder_f32(ray_angle, TAU);
      const float tan_a = tan_f32(ray_angle);

      // Vertical
      float vertical_x;
      float vertical_dx;
      float vertical_dy;
      bool facing_right = cos_f32(ray_angle) > 0 ? true : false;
      if (facing_right) {
        // Right
        vertical_x = (player_tile_pos.x + 1) * TILE;
        vertical_dx = TILE;
        vertical_dy = TILE * tan_a;
      } else {
        // Left
        vertical_x = player_tile_pos.x * TILE;
        vertical_dx = -TILE;
        vertical_dy = -TILE * tan_a;
      }

      const float vertical_y =
          static_cast<float>(player_pos.y) -
          (static_cast<float>(player_pos.x) - vertical_x) * tan_a;

      float vertical_intersection_X = vertical_x;
      float vertical_intersection_Y = vertical_y;

      while (vertical_intersection_X >= .0f &&
             vertical_intersection_X < SCREEN_W &&
             vertical_intersection_Y >= .0f &&
             vertical_intersection_Y < SCREEN_H) {
        const point_t tmp = {floor_f32_to_i32(vertical_intersection_X),
                             floor_f32_to_i32(vertical_intersection_Y)};

        float hit = false;
        int x = tmp.x / TILE;
        int y = tmp.y / TILE;

        assert(x >= 0);
        assert(y >= 0);
        assert(y < SCREEN_H / TILE);

        if (facing_right) {
          // Right
          assert(x < SCREEN_W / TILE);
          if (minimap[y][x] == 1) { hit = true; }

        } else {
          // Left
          assert(x - 1 < SCREEN_W / TILE);
          if (minimap[y][x - 1] == 1) { hit = true; }
        }

        if (hit) {
          // HIT THE WALL
          break;
        }

        vertical_intersection_X += vertical_dx;
        vertical_intersection_Y += vertical_dy;
      }

      // Horizontal
      float horizontal_y;
      float horizontal_dx;
      float horizontal_dy;
      bool facing_up = ray_angle > 0 ? false : true;

      if (facing_up) {
        horizontal_y = player_tile_pos.y * TILE;
        horizontal_dy = -TILE;
        horizontal_dx = -TILE / tan_a;
      } else {
        horizontal_y = (player_tile_pos.y + 1) * TILE;
        horizontal_dy = TILE;
        horizontal_dx = TILE / tan_a;
      }

      const float horizontal_x =
          static_cast<float>(player_pos.x) -
          (static_cast<float>(player_pos.y) - horizontal_y) / tan_a;

      float horizontal_intersection_X = horizontal_x;
      float horizontal_intersection_Y = horizontal_y;

      int counter = 0;
      while (horizontal_intersection_X >= .0f &&
             horizontal_intersection_X < SCREEN_W &&
             horizontal_intersection_Y >= .0f &&
             horizontal_intersection_Y < SCREEN_H && counter < 1000) {
        ++counter;
        const point_t tmp = {static_cast<int>(horizontal_intersection_X),
                             static_cast<int>(horizontal_intersection_Y)};

        float hit = false;
        int x = tmp.x / TILE;
        int y = tmp.y / TILE;

        assert(x >= 0);
        assert(y >= 0);
        assert(x < SCREEN_W / TILE);

        if (facing_up) {
          // Right
          assert(y - 1 < SCREEN_H / TILE);
          if (minimap[y - 1][x] == 1) { hit = true; }
        } else {
          // Left
          assert(y < SCREEN_H / TILE);
          if (minimap[y][x] == 1) { hit = true; }
        }

        if (hit) {
          // HIT THE WALL
          break;
        }

        horizontal_intersection_X += horizontal_dx;
        horizontal_intersection_Y += horizontal_dy;
      }

      // Draw the shorter line:
      float horizontal_len =
          dist_f32(player_pos.x, player_pos.y, horizontal_intersection_X,
                   horizontal_intersection_Y, ray_angle);
      float vertical_len =
          dist_f32(player_pos.x, player_pos.y, vertical_intersection_X,
                   vertical_intersection_Y, ray_angle);

      point_t final_position;
      if (horizontal_len < vertical_len) {
        final_position = {floor_f32_to_i32(horizontal_intersection_X),
                          floor_f32_to_i32(horizontal_intersection_Y)};
      } else {
        final_position = {floor_f32_to_i32(vertical_intersection_X),
                          floor_f32_to_i32(vertical_intersection_Y)};
      }

      const point_t a = player.pixel_pos;
      const point_t b = final_position;
      draw_line(a, b, 0xFFFF00FF);
      draw_circle(final_position.x, final_position.y, 2, 0xFF0000FF);
    }
  }

  void draw_fps()
  {
    const texture_t fps = create_text("FPS: " + STR(FPS()), 0xFF0000FF);

    draw_texture(fps, SCREEN_W - fps.w, 2);
  }

  void draw_2d_map()
  {
    const pixel_t white = 0xFFFFFFFF;
    for (int i = 0; i < SCREEN_H / TILE; ++i) {
      for (int j = 0; j < SCREEN_W / TILE; ++j) {
        if (minimap[i][j] == 1) {
          const rect_t rect = {j * TILE, i * TILE, TILE, TILE};
          draw_rect_outline(rect, white);
        }
      }
    }
  }

  void draw_2d_player()
  {
    const pixel_t green = 0x00FF00FF;
    const pixel_t gray = 0x555555FF;
    const point_t tile = player.tile_pos();

    // Tile
    draw_rect({(tile.x * TILE) + 1, (tile.y * TILE), TILE - 2, TILE - 2}, gray);

    // Direction
    const point_t a = {player.pixel_pos.x, player.pixel_pos.y};
    const point_t b = {
        round_f32_to_i32(player.pixel_pos.x + SCREEN_W * cos_f32(player.angle)),
        round_f32_to_i32(player.pixel_pos.y +
                         SCREEN_W * sin_f32(player.angle))};

    draw_line(a, b, 0xFF0000FF);

    // Body
    draw_circle(player.pixel_pos.x, player.pixel_pos.y, 10, green);

    // Print angle
    const texture_t angle = create_text("A: " + STR(player.angle), 0xFF0000FF);
    draw_texture(angle, 10, 10);
  }


  bool player_legal_move(int x, int y)
  {
    const point_t tile = {x / TILE, y / TILE};

    return (minimap[tile.y][tile.x] == 1) ? false : true;
  }


  void update_player_pos()
  {
    const float sin = sin_f32(player.angle);
    const float cos = cos_f32(player.angle);
    const float speed = PLAYER_SPEED * delta_time();
    const float speed_sin = speed * sin;
    const float speed_cos = speed * cos;

    float dx = .0f;
    float dy = .0f;

    const inputs_t inputs = inputs_state();

    if (inputs.w.pressed) {
      dx += speed_cos;
      dy += speed_sin;
    }
    if (inputs.s.pressed) {
      dx -= speed_cos;
      dy -= speed_sin;
    }
    if (inputs.d.pressed) {
      dx -= speed_sin;
      dy += speed_cos;
    }
    if (inputs.a.pressed) {
      dx += speed_sin;
      dy -= speed_cos;
    }

    // TODO(max): Normalize direction to avoid the diagonal to be faster

    int dest_x = player.pixel_pos.x + round_f32_to_i32(dx);
    int dest_y = player.pixel_pos.y + round_f32_to_i32(dy);

    if (player_legal_move(dest_x, dest_y)) {
      player.pixel_pos.x = dest_x;
      player.pixel_pos.y = dest_y;
    }

    if (inputs.left.pressed) {
      player.angle -= PLAYER_ROTATION_SPEED * delta_time();
    }
    if (inputs.right.pressed) {
      player.angle += PLAYER_ROTATION_SPEED * delta_time();
    }

    // the angle must be between 0 and 2PI. TAU = 2 * pi
    player.angle = remainder_f32(player.angle, TAU);
  }

  void check_events() {}

  void update()
  {
    // Update the player pos
    update_player_pos();
  }


  void draw()
  {
    clear_screen(0x00000000);

    draw_2d_map();
    ray_cast();
    draw_2d_player();

    draw_fps();
  }

  void on_init(void*) override
  {
    player.pixel_pos = {400, 300};
    player.angle = -1.5708;  // 90 degrees
  }

  void on_update(void*) override
  {
    check_events();
    update();
    draw();
  }

  inline void log(const std::string& msg) override { LOG_E << msg << END_E; }
};

int main(int, char**)
{
  gui_t gui;

  if (!gui.run()) { return EXIT_FAILURE; }

  return EXIT_SUCCESS;
}