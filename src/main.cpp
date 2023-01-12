#include <cassert>
#include <cstring>
#include <map>
#include <pixello.hpp>
#include "log.hpp"
#include "math.hpp"

constexpr float TILE_SIZE = 2.0f;    // In meters
constexpr float PLAYER_SIZE = 1.0f;  // In meters
constexpr float PLAYER_HALF_SIZE = TO_F32(PLAYER_SIZE) / 2.0f;
constexpr float PIXELS_IN_METER = 32;

constexpr float PLAYER_SPEED = 0.00000001f;  // In meters per second
constexpr float PLAYER_ROTATION_SPEED = 0.000000001f;
constexpr float MOUSE_SENSITIVITY = 0.0000000003f;

constexpr float FOW = PI / 3.0f;
constexpr float HALF_FOW = FOW / 2.0f;
constexpr int WALL_CHUNK_WIDTH = 2;

const float DARKNESS_MASK_SLOPE = 1.0f * 255.0f / 1100.0f;


constexpr int MAP_W = 32;
constexpr int MAP_H = 24;
// clang-format off
char game_map[MAP_H][MAP_W] = {
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
  point_f32_t position;
  float angle;
};


struct actor_t
{
  point_f32_t position;
  std::string texture_index;
  float scale;
  float height_shift;
};


struct drawable_t
{
  float depth;
  texture_t texture;
  rect_t draw_rect;
  bool to_clip;
  rect_t texture_clip;

  drawable_t(const float _depth,
             const texture_t _texture,
             const rect_t _draw_rect,
             const rect_t _texture_clip)
      : depth(_depth),
        texture(std::move(_texture)),
        draw_rect(std::move(_draw_rect)),
        to_clip(true),
        texture_clip(std::move(_texture_clip))
  {}

  drawable_t(const float _depth,
             const texture_t _texture,
             const rect_t _draw_rect)
      : depth(_depth),
        texture(std::move(_texture)),
        draw_rect(std::move(_draw_rect)),
        to_clip(false)
  {}
};


class gui_t : public pixello
{
private:
  const int screen_w;
  const int screen_h;
  const int num_of_rays;
  const float delta_angle;
  const float draw_scale;
  const float screen_dist;
  const int num_of_columns_in_tile;

  player_t player;
  std::vector<actor_t> actors;
  std::vector<drawable_t> drawables;

  std::map<char, texture_t> textures;
  std::map<std::string, texture_t> static_sprites;

  int map_w;
  int map_h;

public:
  gui_t(const int _screen_w, const int _screen_h)
      : pixello(_screen_w,
                _screen_h,
                "Raystorm",
                60,
                "assets/font/PressStart2P.ttf",
                10),
        screen_w(_screen_w),
        screen_h(_screen_h),
        num_of_rays(_screen_w / WALL_CHUNK_WIDTH),
        delta_angle(FOW / TO_F32(num_of_rays)),
        draw_scale(_screen_w / num_of_rays),
        screen_dist(_screen_w * 30),
        num_of_columns_in_tile(PIXELS_IN_METER / draw_scale)
  {}


private:
  inline point_t pos_to_tile(const float x, const float y) const
  {
    const point_t tile = {floor_f32_to_i32(x / TILE_SIZE),
                          floor_f32_to_i32(y / TILE_SIZE)};

    return tile;
  }


  inline point_t pos_to_tile(const point_f32_t& p) const
  {
    return pos_to_tile(p.x, p.y);
  }


  inline bool is_postion_legal(const float x, const float y) const
  {
    const point_t tile = pos_to_tile(x, y);

    return (game_map[tile.y][tile.x] != 0) ? false : true;
  }


  void check_events()
  {
    if (is_key_pressed(keycap_t::ESC)) {
      // Quit the game
      stop();
    }
  }


  void on_init(void*) override
  {
    // Hide the mouse
    // show_mouse(false);
    // mouse_set_FPS_mode(true);
    drawables.reserve(screen_w * 2);

    // Load textures
    textures[1] = load_image("assets/textures/1.png");
    textures[2] = load_image("assets/textures/2.png");
    textures[3] = load_image("assets/textures/3.png");
    textures[4] = load_image("assets/textures/4.png");
    textures[5] = load_image("assets/textures/5.png");
    textures[-1] = load_image("assets/textures/sky.png");

    // Load static sprites
    static_sprites["candelabrum"] =
        load_image("assets/sprites/static_sprites/candelabrum.png");

    // Init actors
    const actor_t candelabrum = {{3.0f, 3.0f}, "candelabrum", 1.0f, 0.5f};
    actors.push_back(candelabrum);

    // Init player
    player.position = {3.0f, 3.0f};
    player.angle = -1.5708;  // 90 degrees
  }


  void update()
  {
    const float sin = sin_f32(player.angle);
    const float cos = cos_f32(player.angle);
    const float speed = PLAYER_SPEED * delta_time();
    const float speed_sin = speed * sin;
    const float speed_cos = speed * cos;

    float dx = .0f;
    float dy = .0f;

    if (is_key_pressed(keycap_t::W)) {
      dx += speed_cos;
      dy += speed_sin;
    }
    if (is_key_pressed(keycap_t::S)) {
      dx -= speed_cos;
      dy -= speed_sin;
    }
    if (is_key_pressed(keycap_t::D)) {
      dx -= speed_sin;
      dy += speed_cos;
    }
    if (is_key_pressed(keycap_t::A)) {
      dx += speed_sin;
      dy -= speed_cos;
    }

    // TODO(max): Normalize direction to avoid the diagonal to be faster
    const float dest_x = player.position.x + dx;
    const float dest_y = player.position.y + dy;

    // Add player body size
    // TODO(max): Fix the collision with something that works properly
    float x_to_test;
    float y_to_test;

    if (dx > 0) {
      x_to_test = dest_x + PLAYER_HALF_SIZE;
    } else {
      x_to_test = dest_x - PLAYER_HALF_SIZE;
    }

    if (dy > 0) {
      y_to_test = dest_y + PLAYER_HALF_SIZE;
    } else {
      y_to_test = dest_y - PLAYER_HALF_SIZE;
    }

    if (is_postion_legal(x_to_test, y_to_test)) {
      player.position.x = dest_x;
      player.position.y = dest_y;
    }

    // Handle player angle with keys
    if (is_key_pressed(keycap_t::LEFT)) {
      player.angle -= PLAYER_ROTATION_SPEED * delta_time();
    }
    if (is_key_pressed(keycap_t::RIGHT)) {
      player.angle += PLAYER_ROTATION_SPEED * delta_time();
    }

    // Handle player angle with mouse
    const mouse_t mouse = mouse_state();
    player.angle += mouse.relative_x * MOUSE_SENSITIVITY * delta_time();

    // the angle must be between 0 and 2PI. TAU = 2 * pi
    player.angle = remainder_f32(player.angle, TAU);
  }


  void draw_fps()
  {
    const texture_t fps = create_text("FPS: " + STR(FPS()), 0xFF0000FF);

    draw_texture(fps, screen_w - fps.w, 2);
  }


  void draw_2d_map()
  {
    constexpr int PIXELS_IN_TILE = 32;

    const pixel_t white = 0xFFFFFFFF;
    for (int i = 0; i < map_h; ++i) {
      for (int j = 0; j < map_w; ++j) {
        if (game_map[i][j] != 0) {
          const rect_t rect = {j * PIXELS_IN_TILE, i * PIXELS_IN_TILE,
                               PIXELS_IN_TILE, PIXELS_IN_TILE};
          draw_rect_outline(rect, white);
        }
      }
    }

    for (const auto& A : actors) {
      const float x = A.position.x / TILE_SIZE * PIXELS_IN_TILE;
      const float y = A.position.y / TILE_SIZE * PIXELS_IN_TILE;
      draw_circle(floor_f32_to_i32(x), floor_f32_to_i32(y), 5, 0xFFFF00FF);
    }
  }


  void draw_2d_player()
  {
    constexpr int PIXELS_IN_TILE = 32;

    const pixel_t green = 0x00FF00FF;
    const pixel_t gray = 0x555555FF;
    const point_t tile = pos_to_tile(player.position);
    const point_t player_screen_pos = {
        round_f32_to_i32(player.position.x / TILE_SIZE * PIXELS_IN_TILE),
        round_f32_to_i32(player.position.y / TILE_SIZE * PIXELS_IN_TILE)};

    // Tile
    draw_rect({(tile.x * PIXELS_IN_TILE) + 1, (tile.y * PIXELS_IN_TILE),
               PIXELS_IN_TILE - 2, PIXELS_IN_TILE - 2},
              gray);

    // Direction
    const point_t a = {player_screen_pos.x, player_screen_pos.y};
    const point_t b = {round_f32_to_i32(player_screen_pos.x +
                                        screen_w * cos_f32(player.angle)),
                       round_f32_to_i32(player_screen_pos.y +
                                        screen_w * sin_f32(player.angle))};

    draw_line(a, b, 0xFF0000FF);

    // Body
    const int player_size =
        round_f32_to_i32(PLAYER_SIZE / TILE_SIZE * PIXELS_IN_TILE / 2);
    draw_circle(player_screen_pos.x, player_screen_pos.y, player_size, green);

    // Print angle
    const texture_t angle = create_text("A: " + STR(player.angle), 0xFF0000FF);
    draw_texture(angle, 10, 10);
  }


  void ray_cast()
  {
    const point_f32_t player_pos = player.position;
    const point_t player_tile_pos = pos_to_tile(player_pos);
    const float x_limit = map_w * TILE_SIZE;
    const float y_limit = map_h * TILE_SIZE;
    const int PIXELS_IN_TILE = 32;

    texture_t texture;

    // float ray_angle = player.angle;
    float ray_angle = player.angle - HALF_FOW;
    for (int i = 0; i < num_of_rays; ++i, ray_angle += delta_angle) {
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
      while (
          vertical_intersection_X >= .0f && vertical_intersection_X < x_limit &&
          vertical_intersection_Y >= .0f && vertical_intersection_Y < y_limit) {
        const point_t tmp = {floor_f32_to_i32(vertical_intersection_X),
                             floor_f32_to_i32(vertical_intersection_Y)};

        vertical_hit = false;
        int x = tmp.x / PIXELS_IN_TILE;
        int y = tmp.y / PIXELS_IN_TILE;

        assert(x >= 0);
        assert(y >= 0);
        assert(y < map_h);

        if (facing_right) {
          // Right
          assert(x < map_w);
          const char tile_id = game_map[y][x];
          if (tile_id != 0) {
            vertical_hit = true;
            vertical_tile_id = tile_id;
          }

        } else {
          // Left
          assert(x - 1 < map_w);
          const char tile_id = game_map[y][x - 1];
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
             horizontal_intersection_X < x_limit &&
             horizontal_intersection_Y >= .0f &&
             horizontal_intersection_Y < y_limit) {
        const point_t tmp = {static_cast<int>(horizontal_intersection_X),
                             static_cast<int>(horizontal_intersection_Y)};

        horizontal_hit = false;
        int x = tmp.x / PIXELS_IN_TILE;
        int y = tmp.y / PIXELS_IN_TILE;

        assert(x >= 0);
        assert(y >= 0);
        assert(x < map_w);

        if (facing_up) {
          // Right
          assert(y - 1 < map_h);
          const char tile_id = game_map[y - 1][x];
          if (tile_id != 0) {
            horizontal_hit = true;
            horizontal_tile_id = tile_id;
          }
        } else {
          // Left
          assert(y < map_h);
          const char tile_id = game_map[y][x];
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
          if (offset < 0) { offset = TO_F32(PIXELS_IN_TILE) + offset; }
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
          if (offset < 0) { offset = TO_F32(PIXELS_IN_TILE) + offset; }
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
      const int projection_h = floor_f32_to_i32(screen_dist / depth);
      const float multiplier = offset / draw_scale;
      const int texture_scale = texture.w / num_of_columns_in_tile;
      const rect_t wall_chunk = {floor_f32_to_i32(texture_scale * multiplier),
                                 0, floor_f32_to_i32(draw_scale), texture.h};

      const rect_t draw_position = {
          round_f32_to_i32(i * draw_scale),
          (screen_h / 2) - (projection_h / 2),
          round_f32_to_i32(draw_scale),
          projection_h,
      };

      drawables.emplace_back(depth, texture, draw_position, wall_chunk);
    }
  }


  void draw_background()
  {
    // Draw sky
    const texture_t& t = textures[-1];

    const float shifted_player_angle = player.angle + (TAU / 2);

    // Scale the range of player angle to the range of the texture width
    const int texture_cut_point =
        floor_f32_to_i32(shifted_player_angle * (TO_F32(t.w) / TAU));

    if (texture_cut_point + screen_w > t.w) {
      // LEFT
      const rect_t crop_l = {texture_cut_point, 0, t.w - texture_cut_point,
                             screen_h / 2};
      const rect_t dest_rect_l = {0, 0, crop_l.w, screen_h / 2};
      draw_texture(t, dest_rect_l, crop_l);
      // RIGHT
      const rect_t crop_r = {0, 0, screen_w - crop_l.w, screen_h / 2};
      const rect_t dest_rect_r = {crop_l.w, 0, crop_r.w, screen_h / 2};
      draw_texture(t, dest_rect_r, crop_r);
    } else {
      const rect_t crop = {texture_cut_point, 0, screen_w, screen_h / 2};
      const rect_t dest_rect = {0, 0, screen_w, screen_h / 2};
      draw_texture(t, dest_rect, crop);
    }

    // Floor
    draw_rect({0, screen_h / 2, screen_w, screen_h / 2}, 0x101010FF);
  }


  void draw_drawables()
  {
    // Sort the drawables
    std::sort(drawables.begin(), drawables.end(),
              [](const drawable_t& a, const drawable_t& b) {
                return a.depth > b.depth;
              });

    // Draw
    for (const drawable_t& D : drawables) {
      if (D.to_clip) {
        draw_texture(D.texture, D.draw_rect, D.texture_clip);

        // Draw dark layer depended o the distance
        pixel_t dark_mask = 0x000000FF;
        float darkness = DARKNESS_MASK_SLOPE * D.depth;
        dark_mask.a = (darkness < 255) ? round_f32_to_i32(darkness) : 255;
        draw_rect(D.draw_rect, dark_mask);
      } else {
        draw_texture(D.texture, D.draw_rect);
      }
    }
  }


  void draw()
  {
    drawables.clear();

    ray_cast();
    // prepare_to_draw_actors();

    draw_background();
    draw_drawables();
    // draw_2d_map();
    // draw_2d_player();
    draw_fps();
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
  gui_t gui(1024, 768);

  if (!gui.run()) { return EXIT_FAILURE; }

  return EXIT_SUCCESS;
}