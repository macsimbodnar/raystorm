#include <cassert>
#include <map>
#include <pixello.hpp>
#include "log.hpp"
#include "math.hpp"

//  640×480, 800×600, 960×720, 1024×768, 1280×960, 1400×1050, 1440×1080 ,
//  1600×1200, 1856×1392, 1920×1440, and 2048×1536
constexpr int SCREEN_W = 1024;
constexpr int SCREEN_H = 768;
constexpr int PIXELS_IN_TILE = 32;
constexpr float PLAYER_SPEED = 0.0000001;
constexpr float PLAYER_ROTATION_SPEED = 0.000000001;

constexpr float FOW = PI / 3;
constexpr float HALF_FOW = FOW / 2;
constexpr int NUM_RAYS = SCREEN_W / 2;
constexpr float DELTA_ANGLE = FOW / static_cast<float>(NUM_RAYS);
constexpr float SCALE = SCREEN_W / NUM_RAYS;
constexpr int NUM_OF_COLUMNS_IN_TILE = PIXELS_IN_TILE / SCALE;
constexpr float SCREEN_DIST = SCREEN_W * 30;


const float DARKNESS_MASK_SLOPE = 1.0 * 255 / 1100;

constexpr int MAP_W = 32;
constexpr int MAP_H = 24;
constexpr float MAP_W_IN_PIXELS = MAP_W * PIXELS_IN_TILE;
constexpr float MAP_H_IN_PIXELS = MAP_H * PIXELS_IN_TILE;

// clang-format off
// char minimap[MAP_H][MAP_W] = {
//     {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
// };


char minimap[MAP_H][MAP_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,4,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,5,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
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
    const point_t tile = {floor_f32_to_i32(pixel_pos.x / PIXELS_IN_TILE),
                          floor_f32_to_i32(pixel_pos.y / PIXELS_IN_TILE)};
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
    point_t res = {p.x * PIXELS_IN_TILE, p.y * PIXELS_IN_TILE};
    return res;
  }

  void ray_cast()
  {
    const point_f32_t player_pos = player.pixel_pos;
    const point_t player_tile_pos = player.tile_pos();

    texture_t texture;

    // float ray_angle = player.angle;
    float ray_angle = player.angle - HALF_FOW;
    for (int i = 0; i < NUM_RAYS; ++i, ray_angle += DELTA_ANGLE) {
      ray_angle = remainder_f32(ray_angle, TAU);
      const float tan_a = tan_f32(ray_angle);
      char final_tile_id = 0;

      // Vertical
      float vertical_x;
      float vertical_dx;
      float vertical_dy;
      bool facing_right = cos_f32(ray_angle) > 0 ? true : false;
      if (facing_right) {
        // Right
        vertical_x = (player_tile_pos.x + 1) * PIXELS_IN_TILE;
        vertical_dx = PIXELS_IN_TILE;
        vertical_dy = PIXELS_IN_TILE * tan_a;
      } else {
        // Left
        vertical_x = player_tile_pos.x * PIXELS_IN_TILE;
        vertical_dx = -PIXELS_IN_TILE;
        vertical_dy = -PIXELS_IN_TILE * tan_a;
      }

      const float vertical_y =
          player_pos.y - (player_pos.x - vertical_x) * tan_a;

      float vertical_intersection_X = vertical_x;
      float vertical_intersection_Y = vertical_y;

      char vertical_tile_id = 0;

      float vertical_hit = false;
      while (vertical_intersection_X >= .0f &&
             vertical_intersection_X < MAP_W_IN_PIXELS &&
             vertical_intersection_Y >= .0f &&
             vertical_intersection_Y < MAP_H_IN_PIXELS) {
        const point_t tmp = {floor_f32_to_i32(vertical_intersection_X),
                             floor_f32_to_i32(vertical_intersection_Y)};

        vertical_hit = false;
        int x = tmp.x / PIXELS_IN_TILE;
        int y = tmp.y / PIXELS_IN_TILE;

        assert(x >= 0);
        assert(y >= 0);
        assert(y < MAP_H);

        if (facing_right) {
          // Right
          assert(x < MAP_W);
          const char tile_id = minimap[y][x];
          if (tile_id != 0) {
            vertical_hit = true;
            vertical_tile_id = tile_id;
          }

        } else {
          // Left
          assert(x - 1 < MAP_W);
          const char tile_id = minimap[y][x - 1];
          if (tile_id != 0) {
            vertical_hit = true;
            vertical_tile_id = tile_id;
          }
        }

        if (vertical_hit) {
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
        horizontal_y = player_tile_pos.y * PIXELS_IN_TILE;
        horizontal_dy = -PIXELS_IN_TILE;
        horizontal_dx = -PIXELS_IN_TILE / tan_a;
      } else {
        horizontal_y = (player_tile_pos.y + 1) * PIXELS_IN_TILE;
        horizontal_dy = PIXELS_IN_TILE;
        horizontal_dx = PIXELS_IN_TILE / tan_a;
      }

      const float horizontal_x =
          player_pos.x - (player_pos.y - horizontal_y) / tan_a;

      float horizontal_intersection_X = horizontal_x;
      float horizontal_intersection_Y = horizontal_y;

      char horizontal_tile_id = 0;

      float horizontal_hit = false;
      while (horizontal_intersection_X >= .0f &&
             horizontal_intersection_X < MAP_W_IN_PIXELS &&
             horizontal_intersection_Y >= .0f &&
             horizontal_intersection_Y < MAP_H_IN_PIXELS) {
        const point_t tmp = {static_cast<int>(horizontal_intersection_X),
                             static_cast<int>(horizontal_intersection_Y)};

        horizontal_hit = false;
        int x = tmp.x / PIXELS_IN_TILE;
        int y = tmp.y / PIXELS_IN_TILE;

        assert(x >= 0);
        assert(y >= 0);
        assert(x < MAP_W);

        if (facing_up) {
          // Right
          assert(y - 1 < MAP_H);
          const char tile_id = minimap[y - 1][x];
          if (tile_id != 0) {
            horizontal_hit = true;
            horizontal_tile_id = tile_id;
          }
        } else {
          // Left
          assert(y < MAP_H);
          const char tile_id = minimap[y][x];
          if (tile_id != 0) {
            horizontal_hit = true;
            horizontal_tile_id = tile_id;
          }
        }

        if (horizontal_hit) {
          // HIT THE WALL
          break;
        }

        horizontal_intersection_X += horizontal_dx;
        horizontal_intersection_Y += horizontal_dy;
      }

      // Select the shorter line:
      float horizontal_len =
          dist_f32(player_pos.x, player_pos.y, horizontal_intersection_X,
                   horizontal_intersection_Y, ray_angle);
      float vertical_len =
          dist_f32(player_pos.x, player_pos.y, vertical_intersection_X,
                   vertical_intersection_Y, ray_angle);

      point_t final_position;
      float depth;
      float offset;

      if (horizontal_len < vertical_len) {
        // Horizontal is shorter
        final_position = {floor_f32_to_i32(horizontal_intersection_X),
                          floor_f32_to_i32(horizontal_intersection_Y)};
        depth = horizontal_len;

        const float sin = sin_f32(ray_angle);
        if (sin > 0) {
          offset = PIXELS_IN_TILE -
                   remainder_f32(horizontal_intersection_X, PIXELS_IN_TILE);
          if (offset > PIXELS_IN_TILE) { offset = offset - PIXELS_IN_TILE; }
        } else {
          offset = remainder_f32(horizontal_intersection_X, PIXELS_IN_TILE);
          if (offset < 0) {
            offset = static_cast<float>(PIXELS_IN_TILE) + offset;
          }
        }

        final_tile_id = horizontal_tile_id;

      } else {
        // Vertical is shorter
        final_position = {floor_f32_to_i32(vertical_intersection_X),
                          floor_f32_to_i32(vertical_intersection_Y)};
        depth = vertical_len;

        const float cos = cos_f32(ray_angle);
        if (cos > 0) {
          offset = remainder_f32(vertical_intersection_Y, PIXELS_IN_TILE);
          if (offset < 0) {
            offset = static_cast<float>(PIXELS_IN_TILE) + offset;
          }
        } else {
          offset = PIXELS_IN_TILE -
                   remainder_f32(vertical_intersection_Y, PIXELS_IN_TILE);
          if (offset > PIXELS_IN_TILE) { offset = offset - PIXELS_IN_TILE; }
        }

        final_tile_id = vertical_tile_id;
      }


      assert(final_tile_id <= 5);
      assert(final_tile_id >= 0);
      texture = textures[final_tile_id];

      // Removing the fishbowl effect
      depth *= cos_f32(player.angle - ray_angle);

      // Draw textured walls
      const int projection_h = floor_f32_to_i32(SCREEN_DIST / depth);
      const float multiplier = offset / SCALE;
      const int texture_scale = texture.w / NUM_OF_COLUMNS_IN_TILE;
      const rect_t wall_chunk = {floor_f32_to_i32(texture_scale * multiplier),
                                 0, floor_f32_to_i32(SCALE), texture.h};

      const rect_t draw_position = {
          round_f32_to_i32(i * SCALE),
          (SCREEN_H / 2) - (projection_h / 2),
          round_f32_to_i32(SCALE),
          projection_h,
      };

      draw_texture(texture, draw_position, wall_chunk);

      // Draw dark layer depended o the distance
      pixel_t dark_mask = 0x000000FF;
      float darkness = DARKNESS_MASK_SLOPE * depth;
      dark_mask.a = (darkness < 255) ? round_f32_to_i32(darkness) : 255;
      draw_rect(draw_position, dark_mask);
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
    for (int i = 0; i < MAP_H; ++i) {
      for (int j = 0; j < MAP_W; ++j) {
        if (minimap[i][j] == 1) {
          const rect_t rect = {j * PIXELS_IN_TILE, i * PIXELS_IN_TILE,
                               PIXELS_IN_TILE, PIXELS_IN_TILE};
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
    draw_rect({(tile.x * PIXELS_IN_TILE) + 1, (tile.y * PIXELS_IN_TILE),
               PIXELS_IN_TILE - 2, PIXELS_IN_TILE - 2},
              gray);

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
    const point_t tile = {round_f32_to_i32(x) / PIXELS_IN_TILE,
                          round_f32_to_i32(y) / PIXELS_IN_TILE};

    return (minimap[tile.y][tile.x] != 0) ? false : true;
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

  void update() { update_player_pos(); }


  void draw()
  {
    draw_rect({0, SCREEN_H / 2, SCREEN_W, SCREEN_H / 2}, 0x101010FF);
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
    textures[1] = load_image("assets/textures/1.png");
    textures[2] = load_image("assets/textures/2.png");
    textures[3] = load_image("assets/textures/3.png");
    textures[4] = load_image("assets/textures/4.png");
    textures[5] = load_image("assets/textures/5.png");
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