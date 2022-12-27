#include <cassert>
#include <map>
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
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};
// clang-format on

struct point_f32_t
{
  float x;
  float y;
};

struct player_t
{
  point_f32_t pixel_pos;
  float angle;

  point_t tile_pos() const
  {
    const point_t tile = {floor_f32_to_i32(pixel_pos.x / TILE),
                          floor_f32_to_i32(pixel_pos.y / TILE)};
    return tile;
  }
};

class gui_t : public pixello
{
private:
  player_t player;
  std::map<char, texture_t> textures;

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
    const point_f32_t player_pos = player.pixel_pos;
    const point_t player_tile_pos = player.tile_pos();
    const float screen_dist = SCREEN_W * 30;
    const float scale = SCREEN_W / NUM_RAYS;
    const pixel_t color = 0xFFFFFFFF;
    const uint8_t color_decrement = 10;

    const texture_t& texture = textures[1];
    const int num_of_columns_in_tile = TILE / scale;
    const int texture_scale = texture.w / num_of_columns_in_tile;

    // float ray_angle = player.angle;
    float ray_angle = player.angle - HALF_FOW;
    for (int i = 0; i < NUM_RAYS; ++i, ray_angle += DELTA_ANGLE) {
      pixel_t horizontal_color = color;
      pixel_t vertical_color = color;
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
          player_pos.y - (player_pos.x - vertical_x) * tan_a;

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

        if (vertical_color.r > color_decrement) {
          vertical_color.r -= color_decrement;
        }
        if (vertical_color.g > color_decrement) {
          vertical_color.g -= color_decrement;
        }
        if (vertical_color.b > color_decrement) {
          vertical_color.b -= color_decrement;
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
          player_pos.x - (player_pos.y - horizontal_y) / tan_a;

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

        if (horizontal_color.r > color_decrement) {
          horizontal_color.r -= color_decrement;
        }
        if (horizontal_color.g > color_decrement) {
          horizontal_color.g -= color_decrement;
        }
        if (horizontal_color.b > color_decrement) {
          horizontal_color.b -= color_decrement;
        }
      }

      // Draw the shorter line:
      float horizontal_len =
          dist_f32(player_pos.x, player_pos.y, horizontal_intersection_X,
                   horizontal_intersection_Y, ray_angle);
      float vertical_len =
          dist_f32(player_pos.x, player_pos.y, vertical_intersection_X,
                   vertical_intersection_Y, ray_angle);

      point_t final_position;
      float depth;
      pixel_t final_color;
      float offset;

      if (horizontal_len < vertical_len) {
        // Horizontal is shorter
        final_position = {floor_f32_to_i32(horizontal_intersection_X),
                          floor_f32_to_i32(horizontal_intersection_Y)};
        depth = horizontal_len;
        final_color = horizontal_color;

        const float sin = sin_f32(ray_angle);
        if (sin > 0) {
          offset = TILE - remainder_f32(horizontal_intersection_X, TILE);
          if (offset > TILE) { offset = offset - TILE; }
        } else {
          offset = remainder_f32(horizontal_intersection_X, TILE);
          if (offset < 0) { offset = static_cast<float>(TILE) + offset; }
        }
      } else {
        // Vertical is shorter
        final_position = {floor_f32_to_i32(vertical_intersection_X),
                          floor_f32_to_i32(vertical_intersection_Y)};
        depth = vertical_len;
        final_color = vertical_color;

        const float cos = cos_f32(ray_angle);
        if (cos > 0) {
          offset = remainder_f32(vertical_intersection_Y, TILE);
          if (offset < 0) { offset = static_cast<float>(TILE) + offset; }
        } else {
          offset = TILE - remainder_f32(vertical_intersection_Y, TILE);
          if (offset > TILE) { offset = offset - TILE; }
        }
      }

      // Draw 2d Rays
      // const point_t a = player.pixel_pos;
      // const point_t b = final_position;
      // draw_line(a, b, 0xFFFF00FF);
      // draw_circle(final_position.x, final_position.y, 2, 0xFF0000FF);

      // Removing the fishbowl effect
      depth *= cos_f32(player.angle - ray_angle);

      const int projection_h = floor_f32_to_i32(screen_dist / depth);

      // const rect_t wall_chunk = {round_f32_to_i32(i * scale),
      //                            (SCREEN_H / 2) - (projection_h / 2),
      //                            round_f32_to_i32(scale), projection_h};

      // Draw walls
      // draw_rect(wall_chunk, final_color);

      // Draw textured walls
      const float multiplier = offset / scale;
      const rect_t wall_chunk = {floor_f32_to_i32(texture_scale * multiplier),
                                 0, floor_f32_to_i32(scale), texture.h};

      const rect_t draw_position = {
          round_f32_to_i32(i * scale),
          (SCREEN_H / 2) - (projection_h / 2),
          round_f32_to_i32(scale),
          projection_h,
      };

      draw_texture(texture, draw_position, wall_chunk);
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
    const point_t player_pos = {round_f32_to_i32(player.pixel_pos.x),
                                round_f32_to_i32(player.pixel_pos.y)};

    // Tile
    draw_rect({(tile.x * TILE) + 1, (tile.y * TILE), TILE - 2, TILE - 2}, gray);

    // Direction
    const point_t a = {player_pos.x, player_pos.y};
    const point_t b = {
        round_f32_to_i32(player.pixel_pos.x + SCREEN_W * cos_f32(player.angle)),
        round_f32_to_i32(player.pixel_pos.y +
                         SCREEN_W * sin_f32(player.angle))};

    draw_line(a, b, 0xFF0000FF);

    // Body
    draw_circle(player_pos.x, player_pos.y, 10, green);

    // Print angle
    const texture_t angle = create_text("A: " + STR(player.angle), 0xFF0000FF);
    draw_texture(angle, 10, 10);
  }


  bool player_legal_move(float x, float y)
  {
    const point_t tile = {round_f32_to_i32(x) / TILE,
                          round_f32_to_i32(y) / TILE};

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
    const float dest_x = player.pixel_pos.x + dx;
    const float dest_y = player.pixel_pos.y + dy;

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
    // draw_2d_map();
    ray_cast();
    // draw_2d_player();

    draw_fps();
  }

  void on_init(void*) override
  {
    player.pixel_pos = {400, 400};
    player.angle = -1.5708;  // 90 degrees

    // Load textures
    textures[1] = load_image("assets/textures/2.png");
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