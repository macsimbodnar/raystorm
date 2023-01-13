#include <cassert>
#include <cstring>
#include <map>
#include <pixello.hpp>
#include <vector>
#include "log.hpp"
#include "math.hpp"

constexpr float TILE_SIZE = 2.0f;    // In meters
constexpr float PLAYER_SIZE = 1.0f;  // In meters
constexpr float PLAYER_HALF_SIZE = TO_F32(PLAYER_SIZE) / 2.0f;

constexpr float PLAYER_SPEED = 0.00000001f;  // In meters per second
constexpr float PLAYER_ROTATION_SPEED = 0.000000001f;
constexpr float MOUSE_SENSITIVITY = 0.0000000003f;

constexpr float FOV = PI / 3.0f;
constexpr float HALF_FOV = FOV / 2.0f;
constexpr int WALL_CHUNK_WIDTH = 2;

constexpr float DARKNESS_MASK_SLOPE = 1.0f * 255.0f / 80.0f;

constexpr int ANIMATION_DT = 1000 / 6;  // In ms


constexpr int map_w = 32;
constexpr int map_h = 24;
// clang-format off
char game_map[map_h][map_w] = {
    {2,2,2,2,5,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
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

  point_f32_t() : x(0), y(0) {}
  point_f32_t(const float _x, const float _y) : x(_x), y(_y) {}
};


struct player_t
{
  point_f32_t position;
  float angle;
};


struct actor_t
{
  point_f32_t position;
  std::vector<std::string> textures;
  float scale;
  float height_shift;
  int current_texture_index;

  actor_t(const float x,
          const float y,
          std::vector<std::string> _textures,
          const float _scale,
          const float _height_shift)
      : position(x, y),
        textures(std::move(_textures)),
        scale(_scale),
        height_shift(_height_shift),
        current_texture_index(0)
  {}

  actor_t(const float x,
          const float y,
          std::string _texture,
          const float _scale,
          const float _height_shift)
      : position(x, y),
        textures(std::vector<std::string>({_texture})),
        scale(_scale),
        height_shift(_height_shift),
        current_texture_index(0)
  {}
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


struct weapon_t
{
  std::vector<std::string> sprites;
  std::string sound_name;
  int current_sprite_index = 0;
  bool in_fire_animation = false;
  simple_timer timer;
  uint64_t animation_dt = 1000 / 10;  // In ms
};


class gui_t : public pixello
{
private:
  const int screen_w;
  const int screen_h;
  const int num_of_rays;
  const float delta_angle;
  const float screen_dist;
  const int ray_column_width;

  bool should_draw_map;
  player_t player;
  weapon_t weapon;
  std::vector<actor_t> actors;
  std::vector<drawable_t> drawables;

  std::map<char, texture_t> textures;
  std::map<std::string, texture_t> sprites;
  std::map<std::string, texture_t> weapon_sprites;
  std::map<std::string, sound_t> weapon_sounds;

  simple_timer animation_timer;

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
        delta_angle(FOV / TO_F32(num_of_rays)),
        screen_dist(_screen_w / 2.0f / tan_f32(HALF_FOV)),
        ray_column_width(screen_w / num_of_rays),
        should_draw_map(false)
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
    const mouse_t mouse = mouse_state();

    if (is_key_pressed(keycap_t::ESC)) {
      // Quit the game
      stop();
    }

    if (is_key_pressed(keycap_t::U)) {
      // Unlock the mouse
      mouse_set_FPS_mode(false);
    }

    if (is_key_pressed(keycap_t::M)) {
      should_draw_map = true;
    } else {
      should_draw_map = false;
    }

    // Perform animations
    if (animation_timer.get_ticks() > ANIMATION_DT) {
      // Perform the animation and reset the timer

      for (auto& actor : actors) {
        actor.current_texture_index =
            (actor.current_texture_index + 1) % actor.textures.size();
      }

      animation_timer.restart();
    }

    // Weapon

    // Fire trigger
    if ((is_key_pressed(keycap_t::SPACE) || mouse.left_button.click) &&
        !weapon.in_fire_animation) {
      // Start the sound
      play_sound(weapon_sounds[weapon.sound_name]);

      // Start the animation
      weapon.in_fire_animation = true;
      weapon.timer.start();
      weapon.current_sprite_index++;
    }

    // Fire and reload animation
    if (weapon.in_fire_animation) {
      if (weapon.timer.get_ticks() > weapon.animation_dt) {
        const size_t new_index = weapon.current_sprite_index + 1;

        if (new_index >= weapon.sprites.size()) {
          weapon.timer.stop();
          weapon.in_fire_animation = false;
          weapon.current_sprite_index = 0;
        } else {
          weapon.current_sprite_index = new_index;
          weapon.timer.restart();
        }
      }
    }
  }


  void on_init(void*) override
  {
    // Hide the mouse
    // show_mouse(false);
    mouse_set_FPS_mode(true);
    drawables.reserve(screen_w * 2);

    // Load textures
    textures[1] = load_image("assets/textures/1.png");
    textures[2] = load_image("assets/textures/2.png");
    textures[3] = load_image("assets/textures/3.png");
    textures[4] = load_image("assets/textures/4.png");
    textures[5] = load_image("assets/textures/5.png");
    textures[-1] = load_image("assets/textures/sky.png");

    // Load static sprites
    sprites["candelabrum"] =
        load_image("assets/sprites/static_sprites/candelabrum.png");
    sprites["candelabrum_0"] =
        load_image("assets/sprites/animated_sprites/green_light/0.png");
    sprites["candelabrum_1"] =
        load_image("assets/sprites/animated_sprites/green_light/1.png");
    sprites["candelabrum_2"] =
        load_image("assets/sprites/animated_sprites/green_light/3.png");
    sprites["candelabrum_3"] =
        load_image("assets/sprites/animated_sprites/green_light/4.png");
    sprites["candelabrum_r0"] =
        load_image("assets/sprites/animated_sprites/red_light/0.png");
    sprites["candelabrum_r1"] =
        load_image("assets/sprites/animated_sprites/red_light/1.png");
    sprites["candelabrum_r2"] =
        load_image("assets/sprites/animated_sprites/red_light/2.png");
    sprites["candelabrum_r3"] =
        load_image("assets/sprites/animated_sprites/red_light/3.png");


    // Init actors
    actors.emplace_back(
        31.0f, 25.0f,
        std::vector<std::string>({"candelabrum_0", "candelabrum_1",
                                  "candelabrum_2", "candelabrum_3"}),
        1.0f, 0.7f);

    actors.emplace_back(
        27.0f, 25.0f,
        std::vector<std::string>({"candelabrum_r0", "candelabrum_r1",
                                  "candelabrum_r2", "candelabrum_r3"}),
        1.0f, 0.7f);

    actors.emplace_back(27.0f, 19.0f, "candelabrum", 1.0f, 0.7f);

    // Weapon
    weapon_sprites["0"] = load_image("assets/sprites/weapon/shotgun/0.png");
    weapon_sprites["1"] = load_image("assets/sprites/weapon/shotgun/1.png");
    weapon_sprites["2"] = load_image("assets/sprites/weapon/shotgun/2.png");
    weapon_sprites["3"] = load_image("assets/sprites/weapon/shotgun/3.png");
    weapon_sprites["4"] = load_image("assets/sprites/weapon/shotgun/4.png");
    weapon_sprites["5"] = load_image("assets/sprites/weapon/shotgun/5.png");

    weapon_sounds["shotgun"] = load_sound("assets/sound/shotgun.wav");

    weapon.sprites = {"0", "1", "2", "3", "4", "5"};
    weapon.sound_name = "shotgun";

    // Init player
    player.position = {25.0f, 25.0f};
    player.angle = .0f;  // 90 degrees

    animation_timer.start();
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
    // Draw FPS
    const texture_t fps = create_text("FPS: " + STR(FPS()), 0x00FF00FF);
    draw_texture(fps, screen_w - fps.w, 2);

    // Draw player position
    const texture_t player_pos = create_text(
        "X: " + STR(player.position.x) + " Y: " + STR(player.position.y),
        0x00FF00FF);
    draw_texture(player_pos, screen_w - player_pos.w, 2 + fps.h + 2);

    // Draw player angle
    const texture_t angle =
        create_text("ANGLE: " + STR(player.angle), 0x00FF00FF);
    draw_texture(angle, screen_w - angle.w, 2 + fps.h + 2 + player_pos.h + 2);
  }


  void draw_2d_map()
  {
    constexpr int PIXELS_IN_TILE = 32;

    // LOG_I << "H: " << map_h << " W:" << map_w << END_I;

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
  }


  void draw_2d_ray(const float x, const float y)
  {
    constexpr int PIXELS_IN_TILE = 32;

    const point_t a = {
        round_f32_to_i32(player.position.x / TILE_SIZE * PIXELS_IN_TILE),
        round_f32_to_i32(player.position.y / TILE_SIZE * PIXELS_IN_TILE)};
    const point_t b = {round_f32_to_i32(x / TILE_SIZE * PIXELS_IN_TILE),
                       round_f32_to_i32(y / TILE_SIZE * PIXELS_IN_TILE)};

    // LOG_I << "PLAYER  X:" << a.x << " Y:" << a.y << END_I;
    // LOG_I << "INTERS  X:" << b.x << " Y:" << b.y << END_I;

    draw_line(a, b, 0x00FFFFFF);
  }


  void ray_cast()
  {
    const point_f32_t player_pos = player.position;
    const point_t player_tile_pos = pos_to_tile(player_pos);
    const float x_limit = map_w * TILE_SIZE;
    const float y_limit = map_h * TILE_SIZE;

    texture_t texture;

    // float ray_angle = player.angle;
    float ray_angle = player.angle - HALF_FOV;
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
        vertical_x = (player_tile_pos.x + 1) * TILE_SIZE;
        vertical_dx = TILE_SIZE;
        vertical_dy = TILE_SIZE * tan_a;
      } else {
        // Left
        vertical_x = player_tile_pos.x * TILE_SIZE;
        vertical_dx = -TILE_SIZE;
        vertical_dy = -TILE_SIZE * tan_a;
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
        // Iterating over tiles until hit the wall

        vertical_hit = false;
        const int x = floor_f32_to_i32(vertical_intersection_X) / TILE_SIZE;
        const int y = floor_f32_to_i32(vertical_intersection_Y) / TILE_SIZE;

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
          // draw_2d_ray(vertical_intersection_X, vertical_intersection_Y);
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
        horizontal_y = player_tile_pos.y * TILE_SIZE;
        horizontal_dy = -TILE_SIZE;
        horizontal_dx = -TILE_SIZE / tan_a;
      } else {
        horizontal_y = (player_tile_pos.y + 1) * TILE_SIZE;
        horizontal_dy = TILE_SIZE;
        horizontal_dx = TILE_SIZE / tan_a;
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
        // Iterating over tiles until hit the wall

        horizontal_hit = false;
        const int x = floor_f32_to_i32(horizontal_intersection_X) / TILE_SIZE;
        const int y = floor_f32_to_i32(horizontal_intersection_Y) / TILE_SIZE;

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
          // draw_2d_ray(horizontal_intersection_X, horizontal_intersection_Y);
          break;
        }

        horizontal_intersection_X += horizontal_dx;
        horizontal_intersection_Y += horizontal_dy;
      }

      // Select the shorter line:
      const float horizontal_len =
          dist_f32(player_pos.x, player_pos.y, horizontal_intersection_X,
                   horizontal_intersection_Y, ray_angle);

      const float vertical_len =
          dist_f32(player_pos.x, player_pos.y, vertical_intersection_X,
                   vertical_intersection_Y, ray_angle);

      point_t final_position;
      float depth;
      float offset;

      if (horizontal_len < vertical_len) {
        // Horizontal is shorter
        final_position = {floor_f32_to_i32(horizontal_intersection_X),
                          floor_f32_to_i32(horizontal_intersection_Y)};

        // draw_2d_ray(horizontal_intersection_X, horizontal_intersection_Y);
        depth = horizontal_len;

        const float sin = sin_f32(ray_angle);
        if (sin > 0) {
          offset =
              TILE_SIZE - remainder_f32(horizontal_intersection_X, TILE_SIZE);
          if (offset > TILE_SIZE) { offset = offset - TILE_SIZE; }
        } else {
          offset = remainder_f32(horizontal_intersection_X, TILE_SIZE);
          if (offset < 0) { offset = TO_F32(TILE_SIZE) + offset; }
        }

        final_tile_id = horizontal_tile_id;

      } else {
        // Vertical is shorter
        final_position = {floor_f32_to_i32(vertical_intersection_X),
                          floor_f32_to_i32(vertical_intersection_Y)};

        // draw_2d_ray(vertical_intersection_X, vertical_intersection_Y);

        depth = vertical_len;

        const float cos = cos_f32(ray_angle);
        if (cos > 0) {
          offset = remainder_f32(vertical_intersection_Y, TILE_SIZE);
          if (offset < 0) { offset = TO_F32(TILE_SIZE) + offset; }
        } else {
          offset =
              TILE_SIZE - remainder_f32(vertical_intersection_Y, TILE_SIZE);
          if (offset > TILE_SIZE) { offset = offset - TILE_SIZE; }
        }

        final_tile_id = vertical_tile_id;
      }

      assert(final_tile_id <= 5);
      assert(final_tile_id >= 0);

      texture = textures[final_tile_id];

      assert(texture.w > 0);
      assert(texture.h > 0);

      // Removing the fishbowl effect
      depth *= cos_f32(player.angle - ray_angle);

      // Draw textured walls

      // Calculating the current column height
      const int projection_h =
          floor_f32_to_i32(screen_dist / depth * TILE_SIZE);

      const float texture_offset = (texture.w * offset) / TILE_SIZE;

      const rect_t wall_chunk = {floor_f32_to_i32(texture_offset), 0,
                                 floor_f32_to_i32(ray_column_width), texture.h};


      const rect_t draw_position = {
          round_f32_to_i32(i * ray_column_width),
          (screen_h / 2) - (projection_h / 2),
          round_f32_to_i32(ray_column_width),
          projection_h,
      };

      // draw_texture(texture, draw_position, wall_chunk);
      // draw_rect(draw_position, 0xFF0000FF);

      drawables.emplace_back(depth, texture, draw_position, wall_chunk);
    }
  }


  void prepare_to_draw_actors()
  {
    for (const auto& actor : actors) {
      // Calculate the angle that the player looks at the sprite
      const float dx = actor.position.x - player.position.x;
      const float dy = actor.position.y - player.position.y;

      const float theta = atan2_f32(dy, dx);

      // Calculate the difference between the player angle and the theta angle.
      // This will show how many rays the sprite is shifted from the central ray

      float actor_delta_angle = theta - player.angle;

      // Checks if the sprite is on the opposite side of the player
      if ((dx > 0 && player.angle > PI) || (dx < 0 && dy < 0)) {
        actor_delta_angle += TAU;
      }

      // Set the angle in range of TAU
      actor_delta_angle = remainder_f32(actor_delta_angle, TAU);

      // Finding the screen X position of the sprite calculating how many rays
      const float delta_rays = actor_delta_angle / delta_angle;
      const float screen_x =
          ((num_of_rays / 2.0f) + delta_rays) * ray_column_width;

      const float dist = hypot_f32(dx, dy);
      const float norm_dist = dist * cos_f32(actor_delta_angle);

      const texture_t& texture =
          sprites[actor.textures[actor.current_texture_index]];
      const float texture_half_w = texture.w / 2;
      const float texture_ration = TO_F32(texture.w) / TO_F32(texture.h);

      // We get the projection only if the sprite is in the screen
      if ((-texture_half_w) < screen_x &&
          screen_x < (screen_w + texture_half_w) && norm_dist > 0.5f) {
        // Calculating the projection
        const float projection = screen_dist / norm_dist * actor.scale;
        const float projection_w = projection * texture_ration;
        const float projection_h = projection;

        const float h_shift = projection_h * actor.height_shift;
        const int sprite_half_w = floor_f32_to_i32(projection_w / 2);
        const int x = screen_x - sprite_half_w;
        const int y =
            (screen_h / 2) - (floor_f32_to_i32(projection_h / 2)) + h_shift;

        const rect_t draw_rect = {x, y, floor_f32_to_i32(projection_w),
                                  floor_f32_to_i32(projection_h)};

        // draw_texture(texture, draw_rect);
        drawables.emplace_back(norm_dist, texture, draw_rect);
      }
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


  void draw_weapon()
  {
    const texture_t& texture =
        weapon_sprites[weapon.sprites[weapon.current_sprite_index]];

    const int w = texture.w / 3;
    const int h = texture.h / 3;

    const rect_t r = {(screen_w - w) / 2, screen_h - h, w, h};
    draw_texture(texture, r);
  }


  void draw()
  {
    drawables.clear();

    ray_cast();
    prepare_to_draw_actors();

    draw_background();
    draw_drawables();
    draw_weapon();

    if (should_draw_map) {
      draw_2d_map();
      draw_2d_player();
    }
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