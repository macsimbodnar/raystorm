#include <cassert>
#include <cstdlib>
#include <cstring>
#include <map>
#include <pixello.hpp>
#include <vector>
#include "log.hpp"
#include "math.hpp"

static bool reset_game = false;

constexpr float TILE_SIZE = 2.0f;    // In meters
constexpr float PLAYER_SIZE = 0.5f;  // In meters
constexpr float PLAYER_HALF_SIZE = PLAYER_SIZE / 2.0f;

constexpr float PLAYER_SPEED = 0.00000001f;  // In meters per second
constexpr float PLAYER_SPEED_BOOSTED = PLAYER_SPEED * 2.0f;
constexpr float PLAYER_ROTATION_SPEED = 0.0000000025f;
constexpr float MOUSE_SENSITIVITY = 0.0000000009f;

constexpr float FOV = PI / 3.0f;
constexpr float HALF_FOV = FOV / 2.0f;
constexpr int WALL_CHUNK_WIDTH = 2;

constexpr float DARKNESS_MASK_SLOPE = 1.0f * 255.0f / 80.0f;

constexpr int ANIMATION_DT = 1000 / 6;  // In ms


constexpr int MAP_W = 32;
constexpr int MAP_H = 24;
// clang-format off
char MAP[MAP_H][MAP_W] = {
    {2,2,2,2,5,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,3,0,0,0,0,0,2,2,2,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,0,0,0,0,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,3,0,0,0,2,0,2,0,2,0,2,0,0,0,0,1},
    {1,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,3,0,0,0,2,0,0,0,0,0,2,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,3,0,0,0,2,0,2,0,2,0,2,0,0,0,0,1},
    {1,0,0,0,0,0,4,0,0,0,0,0,0,0,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,3,0,0,0,0,0,2,2,2,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,5,0,0,0,0,0,0,0,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
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
  bool boost = false;
};


static int rand_between(const int min, const int max)
{
  int rand_num = (rand() % (max - min + 1)) + min;
  return rand_num;
}


class animation_t
{
public:
  enum type_t
  {
    SINGLE,
    LOOP
  };

private:
  std::vector<std::string> frames;
  size_t current_animation_id = 0;
  simple_timer timer;
  uint64_t timer_th;
  bool running;
  type_t type;

public:
  animation_t() {}
  animation_t(const std::vector<std::string>& _frames,
              const uint64_t _timer_th,
              const type_t _type = SINGLE)
      : frames(_frames),
        current_animation_id(0),
        timer_th(_timer_th),
        running(false),
        type(_type)
  {}

  inline void start_animation()
  {
    timer.start();
    running = true;
  }

  inline void stop_animation()
  {
    running = false;
    timer.stop();
    current_animation_id = 0;
  }

  inline bool is_animation_running() const { return running; }

  inline const std::string& sprite() const
  {
    return frames[current_animation_id];
  }

  inline void animate()
  {
    if (running && timer.get_ticks() >= timer_th) {
      timer.restart();
      const size_t new_animation_id = current_animation_id + 1;
      const size_t num_frames = frames.size();

      switch (type) {
        case LOOP:
          current_animation_id = new_animation_id % num_frames;
          break;

        case SINGLE:
          if (new_animation_id >= num_frames) {
            running = false;
            timer.stop();
            current_animation_id = 0;
          } else {
            current_animation_id = new_animation_id;
          }

          break;
      }
      timer.restart();
    }
  }
};


class base_actor_t
{
private:
  struct animation_and_sound_t
  {
    animation_t animation;
    std::string sound;
  };

  std::map<std::string, animation_and_sound_t> animations;
  std::string current_animation;

public:
  point_f32_t position;
  float scale;
  float height_shift;
  std::string default_sprite;

public:
  base_actor_t(const point_f32_t _position,
               const float _scale,
               const float _height_shift,
               const std::string& _default_sprite)
      : position(_position),
        scale(_scale),
        height_shift(_height_shift),
        default_sprite(_default_sprite)
  {}

  inline void add_animation(const std::string& animation_name,
                            const std::vector<std::string>& frames,
                            const uint64_t timer_th,
                            const animation_t::type_t type,
                            const std::string& sound = "")
  {
    animations[animation_name] = {{frames, timer_th, type}, sound};
  }

  inline void start_animation(const std::string& animation_name)
  {
    stop_animation();
    current_animation = animation_name;
    animations[current_animation].animation.start_animation();
  }

  inline void stop_animation()
  {
    animations[current_animation].animation.start_animation();
    current_animation.clear();
  }

  inline void animate()
  {
    animations[current_animation].animation.animate();
    if (!animations[current_animation].animation.is_animation_running()) {
      current_animation.clear();
    }
  }

  inline const std::string& animation_sound(const std::string& animation_name)
  {
    return animations[animation_name].sound;
  }

  inline const std::string& sprite() const
  {
    if (current_animation.empty()) { return default_sprite; }
    return animations.at(current_animation).animation.sprite();
  }

  inline bool in_animation() const { return !current_animation.empty(); }
};


class unanimated_actor_t : public base_actor_t
{
public:
  unanimated_actor_t(const float x,
                     const float y,
                     const float scale,
                     const float height_shift,
                     const std::string& sprite)
      : base_actor_t({x, y}, scale, height_shift, sprite)
  {}
};


class animated_actor_t : public base_actor_t
{
public:
  animated_actor_t(const float x,
                   const float y,
                   const float scale,
                   const float height_shift)
      : base_actor_t({x, y}, scale, height_shift, "")
  {}
};


class npc_t : public base_actor_t
{
public:
  float attack_distance;
  float speed;
  float size;
  float health;
  float attack_damage;
  float accuracy;
  bool alive;
  bool see_the_player;
  std::string death_sprite;

  enum animation_type_t
  {
    ATTACK,
    DEATH,
    IDLE,
    PAIN,
    WALK
  };

  npc_t(const float x,
        const float y,
        const float scale,
        const float height_shift,
        const std::string& sprite,
        const std::string& _death_sprite)
      : base_actor_t({x, y}, scale, height_shift, sprite),
        attack_distance(rand_between(3, 6)),
        speed(0.03f),
        size(.5f),
        health(100),
        attack_damage(10),
        accuracy(0.15f),
        alive(true),
        see_the_player(false),
        death_sprite(_death_sprite)
  {}

  inline void add_animation(
      const animation_type_t type,
      const std::vector<std::string>& frames,
      const uint64_t timer_th,
      const std::string& sound,
      const animation_t::type_t animation_loop_type = animation_t::SINGLE)
  {
    base_actor_t::add_animation(std::to_string(type), frames, timer_th,
                                animation_loop_type, sound);
  }

  inline void start_animation(const animation_type_t type)
  {
    base_actor_t::start_animation(std::to_string(type));
  }

  inline const std::string& animation_sound(const animation_type_t type)
  {
    return base_actor_t::animation_sound(std::to_string(type));
  }
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
  animation_t animation;
  std::string sound_name;
  float damage;
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

  player_t player;
  weapon_t weapon;
  bool pulled_trigger;

  std::vector<unanimated_actor_t> unanimated_actors;
  std::vector<animated_actor_t> animated_actors;
  std::vector<npc_t> NPCs;

  std::vector<drawable_t> to_draw;
  std::vector<drawable_t> to_draw_floor;
  std::vector<drawable_t> to_draw_ceiling;

  bool should_draw_map;
  int map_w;
  int map_h;
  std::vector<std::vector<char>> game_map;

  std::map<int, texture_t> textures;
  std::map<std::string, texture_t> sprites;

  std::map<std::string, music_t> musics;
  std::map<std::string, sound_t> sounds;

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
        pulled_trigger(false),
        should_draw_map(false)
  {}


private:
  inline bool is_player_visible(const point_f32_t& point) const
  {
    // we ray cast from player to the point x,y and see if there is a wall in
    // between

    const float x_limit = map_w * TILE_SIZE;
    const float y_limit = map_h * TILE_SIZE;

    const point_f32_t& A = player.position;
    const point_t A_tile = pos_to_tile(A);
    const point_f32_t& B = point;
    const point_t B_tile = pos_to_tile(B);
    const float ray_angle = atan2_f32(B.y - A.y, B.x - A.x);
    const float tan_a = tan_f32(ray_angle);

    // Check for vertical
    float vertical_x;
    float vertical_dx;
    float vertical_dy;
    const bool facing_right = cos_f32(ray_angle) > 0 ? true : false;
    if (facing_right) {
      // Right
      vertical_x = (A_tile.x + 1) * TILE_SIZE;
      vertical_dx = TILE_SIZE;
      vertical_dy = TILE_SIZE * tan_a;
    } else {
      // Left
      vertical_x = A_tile.x * TILE_SIZE;
      vertical_dx = -TILE_SIZE;
      vertical_dy = -TILE_SIZE * tan_a;
    }

    const float vertical_y = A.y - (A.x - vertical_x) * tan_a;

    float vertical_intersection_X = vertical_x;
    float vertical_intersection_Y = vertical_y;

    float vertical_hit = false;
    while (
        vertical_intersection_X >= .0f && vertical_intersection_X < x_limit &&
        vertical_intersection_Y >= .0f && vertical_intersection_Y < y_limit) {
      vertical_hit = false;
      const int x = floor_f32_to_i32(vertical_intersection_X) / TILE_SIZE;
      const int y = floor_f32_to_i32(vertical_intersection_Y) / TILE_SIZE;


      assert(x >= 0);
      assert(y >= 0);
      assert(y < map_h);

      if (facing_right) {
        // Right
        assert(x < map_w);

        if (x >= B_tile.x) {
          // Stop checking if pass the point B
          break;
        }

        const char tile_id = game_map[y][x];
        if (tile_id != 0) { vertical_hit = true; }
      } else {
        // Left
        assert(x - 1 < map_w);

        if (x <= B_tile.x) {
          // Stop checking if pass the point B
          break;
        }

        const char tile_id = game_map[y][x - 1];
        if (tile_id != 0) { vertical_hit = true; }
      }

      // If we hit something then just return false
      if (vertical_hit) {
        // HIT THE WALL
        return false;
      }

      vertical_intersection_X += vertical_dx;
      vertical_intersection_Y += vertical_dy;
    }

    // Horizontal
    float horizontal_y;
    float horizontal_dx;
    float horizontal_dy;
    const bool facing_up = ray_angle > 0 ? false : true;

    if (facing_up) {
      horizontal_y = A_tile.y * TILE_SIZE;
      horizontal_dy = -TILE_SIZE;
      horizontal_dx = -TILE_SIZE / tan_a;
    } else {
      horizontal_y = (A_tile.y + 1) * TILE_SIZE;
      horizontal_dy = TILE_SIZE;
      horizontal_dx = TILE_SIZE / tan_a;
    }

    const float horizontal_x = A.x - (A.y - horizontal_y) / tan_a;

    float horizontal_intersection_X = horizontal_x;
    float horizontal_intersection_Y = horizontal_y;

    float horizontal_hit = false;
    while (horizontal_intersection_X >= .0f &&
           horizontal_intersection_X < x_limit &&
           horizontal_intersection_Y >= .0f &&
           horizontal_intersection_Y < y_limit) {
      horizontal_hit = false;
      const int x = floor_f32_to_i32(horizontal_intersection_X) / TILE_SIZE;
      const int y = floor_f32_to_i32(horizontal_intersection_Y) / TILE_SIZE;

      assert(x >= 0);
      assert(y >= 0);
      assert(x < map_w);

      if (facing_up) {
        // Right
        assert(y - 1 < map_h);

        if (y <= B_tile.y) {
          // Stop checking if pass the point B
          break;
        }

        const char tile_id = game_map[y - 1][x];
        if (tile_id != 0) { horizontal_hit = true; }
      } else {
        // Left
        assert(y < map_h);

        if (y >= B_tile.y) {
          // Stop checking if pass the point B
          break;
        }

        const char tile_id = game_map[y][x];
        if (tile_id != 0) { horizontal_hit = true; }
      }

      // If we hit something then just return false
      if (horizontal_hit) {
        // HIT THE WALL
        return false;
      }

      horizontal_intersection_X += horizontal_dx;
      horizontal_intersection_Y += horizontal_dy;
    }

    return true;
  }

  inline void load_map(void* m, const size_t w, const size_t h)
  {
    map_w = w;
    map_h = h;

    // TODO(max): Change this shit
    char(*map)[w] = static_cast<char(*)[w]>(m);

    game_map.resize(h);
    for (size_t i = 0; i < h; i++) {
      game_map[i].resize(w);
      for (size_t j = 0; j < w; j++) {
        game_map[i][j] = static_cast<char>(map[i][j]);
      }
    }
  }

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

    if (is_key_pressed(keycap_t::R)) {
      // Reset the game
      reset_game = true;
      stop();
    }

    if (is_key_pressed(keycap_t::U)) {
      // Unlock the mouse
      mouse_set_FPS_mode(false);
    }

    if (is_key_pressed(keycap_t::LSHIFT)) {
      player.boost = true;
    } else {
      player.boost = false;
    }

    if (is_key_pressed(keycap_t::M)) {
      should_draw_map = true;
    } else {
      should_draw_map = false;
    }

    // Weapon

    // Reset the pulled trigger and set it in case
    pulled_trigger = false;
    if ((is_key_pressed(keycap_t::SPACE) || mouse.left_button_pressed) &&
        !weapon.animation.is_animation_running()) {
      pulled_trigger = true;
    }
  }


  void animate()
  {
    // Perform all the animations

    // Actors
    for (auto& A : animated_actors) {
      A.animate();
    }

    // NPCs
    for (auto& NPC : NPCs) {
      NPC.animate();
    }

    // Fire and reload animation
    if (weapon.animation.is_animation_running()) { weapon.animation.animate(); }
  }


  void on_init(void*) override
  {
    // Hide the mouse
    mouse_set_FPS_mode(true);

    to_draw.clear();
    to_draw_floor.clear();
    to_draw_ceiling.clear();
    unanimated_actors.clear();
    animated_actors.clear();
    NPCs.clear();

    // Reserve the memory for the vectors
    to_draw.reserve(screen_w * 2);
    to_draw_floor.reserve(screen_w * 2);
    to_draw_ceiling.reserve(screen_w * 2);
    unanimated_actors.reserve(10);
    animated_actors.reserve(10);
    NPCs.reserve(10);

    // Load map
    load_map(MAP, MAP_W, MAP_H);

    // Load musics
    musics["main"] = load_music("assets/sound/theme.mp3");

    // Load textures
    // textures[0] = load_image("assets/molesta.webp");
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
    sprites["buzzy"] = load_image("assets/sprites/buzzy.webp");

    // Add unanimated actors
    unanimated_actors.emplace_back(27.0f, 19.0f, 1.5f, 0.3f, "candelabrum");
    unanimated_actors.emplace_back(16.5f * 2, 12.5f * 2, 4.0f, -1.0f, "buzzy");

    // Add animated actors
    animated_actors.emplace_back(31.0f, 25.0f, 1.8f, 0.2f);
    animated_actors.back().add_animation(
        "default_animation",
        {"candelabrum_0", "candelabrum_1", "candelabrum_2", "candelabrum_3"},
        ANIMATION_DT, animation_t::LOOP);
    animated_actors.back().start_animation("default_animation");

    animated_actors.emplace_back(40.0f, 25.0f, 1.8f, 0.2);
    animated_actors.back().add_animation("default_animation",
                                         {"candelabrum_r0", "candelabrum_r1",
                                          "candelabrum_r2", "candelabrum_r3"},
                                         ANIMATION_DT, animation_t::LOOP);
    animated_actors.back().start_animation("default_animation");

    // Weapon
    sprites["shotgun_0"] = load_image("assets/sprites/weapon/shotgun/0.png");
    sprites["shotgun_1"] = load_image("assets/sprites/weapon/shotgun/1.png");
    sprites["shotgun_2"] = load_image("assets/sprites/weapon/shotgun/2.png");
    sprites["shotgun_3"] = load_image("assets/sprites/weapon/shotgun/3.png");
    sprites["shotgun_4"] = load_image("assets/sprites/weapon/shotgun/4.png");
    sprites["shotgun_5"] = load_image("assets/sprites/weapon/shotgun/5.png");

    sounds["shotgun"] = load_sound("assets/sound/shotgun.wav");

    weapon.animation = {{"shotgun_0", "shotgun_1", "shotgun_2", "shotgun_3",
                         "shotgun_4", "shotgun_5"},
                        1000 / 12,
                        animation_t::SINGLE};

    weapon.sound_name = "shotgun";
    weapon.damage = 50.1f;

    // NPCs
    sprites["soldier_default"] = load_image("assets/sprites/npc/soldier/0.png");
    sprites["soldier_attack_0"] =
        load_image("assets/sprites/npc/soldier/attack/0.png");
    sprites["soldier_attack_1"] =
        load_image("assets/sprites/npc/soldier/attack/1.png");
    sprites["soldier_death_0"] =
        load_image("assets/sprites/npc/soldier/death/0.png");
    sprites["soldier_death_1"] =
        load_image("assets/sprites/npc/soldier/death/1.png");
    sprites["soldier_death_2"] =
        load_image("assets/sprites/npc/soldier/death/2.png");
    sprites["soldier_death_3"] =
        load_image("assets/sprites/npc/soldier/death/3.png");
    sprites["soldier_death_4"] =
        load_image("assets/sprites/npc/soldier/death/4.png");
    sprites["soldier_death_5"] =
        load_image("assets/sprites/npc/soldier/death/5.png");
    sprites["soldier_death_6"] =
        load_image("assets/sprites/npc/soldier/death/6.png");
    sprites["soldier_death_7"] =
        load_image("assets/sprites/npc/soldier/death/7.png");
    sprites["soldier_death_8"] =
        load_image("assets/sprites/npc/soldier/death/8.png");
    sprites["soldier_idle_0"] =
        load_image("assets/sprites/npc/soldier/idle/0.png");
    sprites["soldier_idle_1"] =
        load_image("assets/sprites/npc/soldier/idle/1.png");
    sprites["soldier_idle_2"] =
        load_image("assets/sprites/npc/soldier/idle/2.png");
    sprites["soldier_idle_3"] =
        load_image("assets/sprites/npc/soldier/idle/3.png");
    sprites["soldier_idle_4"] =
        load_image("assets/sprites/npc/soldier/idle/4.png");
    sprites["soldier_idle_5"] =
        load_image("assets/sprites/npc/soldier/idle/5.png");
    sprites["soldier_idle_6"] =
        load_image("assets/sprites/npc/soldier/idle/6.png");
    sprites["soldier_idle_7"] =
        load_image("assets/sprites/npc/soldier/idle/7.png");
    sprites["soldier_pain_0"] =
        load_image("assets/sprites/npc/soldier/pain/0.png");
    sprites["soldier_walk_0"] =
        load_image("assets/sprites/npc/soldier/walk/0.png");
    sprites["soldier_walk_1"] =
        load_image("assets/sprites/npc/soldier/walk/1.png");
    sprites["soldier_walk_2"] =
        load_image("assets/sprites/npc/soldier/walk/2.png");
    sprites["soldier_walk_3"] =
        load_image("assets/sprites/npc/soldier/walk/3.png");

    sounds["soldier_attack"] = load_sound("assets/sound/npc_attack.wav");
    sounds["soldier_death"] = load_sound("assets/sound/npc_death.wav");
    sounds["soldier_pain"] = load_sound("assets/sound/npc_pain.wav");

    NPCs.emplace_back(23.0f * TILE_SIZE + TILE_SIZE / 2,
                      7.0f * TILE_SIZE + TILE_SIZE / 2, 1.5f, .3f,
                      "soldier_default", "soldier_death_8");

    NPCs.back().add_animation(npc_t::ATTACK,
                              {"soldier_attack_0", "soldier_attack_1"},
                              ANIMATION_DT, "soldier_attack");

    NPCs.back().add_animation(
        npc_t::DEATH,
        {"soldier_death_0", "soldier_death_1", "soldier_death_2",
         "soldier_death_3", "soldier_death_4", "soldier_death_5",
         "soldier_death_6", "soldier_death_7", "soldier_death_8"},
        ANIMATION_DT / 4, "soldier_death");

    NPCs.back().add_animation(
        npc_t::IDLE,
        {"soldier_idle_0", "soldier_idle_1", "soldier_idle_2", "soldier_idle_3",
         "soldier_idle_4", "soldier_idle_5", "soldier_idle_6",
         "soldier_idle_7"},
        ANIMATION_DT, "", animation_t::LOOP);

    NPCs.back().add_animation(npc_t::PAIN, {"soldier_pain_0"}, ANIMATION_DT,
                              "soldier_pain");

    NPCs.back().add_animation(npc_t::WALK,
                              {"soldier_walk_0", "soldier_walk_1",
                               "soldier_walk_2", "soldier_walk_3"},
                              ANIMATION_DT, "");

    NPCs.back().start_animation(npc_t::IDLE);

    // Add another soldier
    NPCs.push_back(NPCs.back());
    NPCs.back().position = {6.0f, 6.0f};

    NPCs.push_back(NPCs.back());
    NPCs.back().position = {6.0f, 12.0f};

    NPCs.push_back(NPCs.back());
    NPCs.back().position = {21.0f, 19.0f};

    // Init player
    // player.position = {25.0f, 25.0f};
    // player.position = {4.0f, 4.0f};
    player.position = {4.0f, 15.0f};
    player.angle = -P2;

    // Start things
    music_do(music_t::PLAY, musics["main"]);
  }


  inline void update_player()
  {
    // Update the player position
    const float sin = sin_f32(player.angle);
    const float cos = cos_f32(player.angle);
    const float speed =
        delta_time() * (player.boost ? PLAYER_SPEED_BOOSTED : PLAYER_SPEED);
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


  inline void update_weapon()
  {
    // Fire player animation
    if (pulled_trigger) {
      // Start the sound
      play_sound(sounds[weapon.sound_name]);

      // Start the animation
      weapon.animation.start_animation();
    }
  }

  inline void update_NPCs()
  {
    // Update the NPCs
    const float player_angle_cos = cos_f32(player.angle);
    const float player_angle_sin = sin_f32(player.angle);
    for (auto& NPC : NPCs) {
      if (NPC.alive) {
        // Check if see the player
        NPC.see_the_player = is_player_visible(NPC.position);

        // Check for hits
        if (pulled_trigger && NPC.see_the_player) {
          // Calculating if we it the npc using the line and circle formulas
          // distance = abs((Bx - Px) * sin(A) - (By - Py) * cos(A))

          const float x_diff = NPC.position.x - player.position.x;
          const float y_diff = NPC.position.y - player.position.y;

          const float distance =
              abs_f32(x_diff * player_angle_sin - y_diff * player_angle_cos);

          // calculate if the npc is in front or back of the player
          const float angle_between = atan2_f32(y_diff, x_diff);
          const float angle_difference = abs_f32(angle_between - player.angle);
          const bool is_in_front = angle_difference > P2 ? false : true;

          if (is_in_front) {
            if (distance < NPC.size / 2) {
              // We hit the npc
              play_sound(sounds[NPC.animation_sound(npc_t::PAIN)]);
              NPC.start_animation(npc_t::PAIN);
              NPC.health -= weapon.damage;
            }

            // Check if npc is death
            if (NPC.health <= .0f) {
              NPC.alive = false;
              NPC.start_animation(npc_t::DEATH);
              NPC.default_sprite = NPC.death_sprite;
            }
          }
        }

        if (NPC.alive && !NPC.in_animation()) {
          NPC.start_animation(npc_t::IDLE);
        }
      }
    }
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
    const int player_x =
        floor_f32_to_i32(player.position.x / TILE_SIZE * PIXELS_IN_TILE);
    const int player_y =
        floor_f32_to_i32(player.position.y / TILE_SIZE * PIXELS_IN_TILE);

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

    for (const auto& A : unanimated_actors) {
      const point_f32_t& pos = A.position;
      const float x = pos.x / TILE_SIZE * PIXELS_IN_TILE;
      const float y = pos.y / TILE_SIZE * PIXELS_IN_TILE;
      draw_circle(floor_f32_to_i32(x), floor_f32_to_i32(y), 5, 0xFFFF00FF);
    }

    for (const auto& A : animated_actors) {
      const point_f32_t& pos = A.position;
      const float x = pos.x / TILE_SIZE * PIXELS_IN_TILE;
      const float y = pos.y / TILE_SIZE * PIXELS_IN_TILE;
      draw_circle(floor_f32_to_i32(x), floor_f32_to_i32(y), 5, 0xFFFF00FF);
    }

    for (const auto& A : NPCs) {
      const point_f32_t& pos = A.position;
      const int x = floor_f32_to_i32(pos.x / TILE_SIZE * PIXELS_IN_TILE);
      const int y = floor_f32_to_i32(pos.y / TILE_SIZE * PIXELS_IN_TILE);

      // Draw line if the NPC see the player
      if (A.see_the_player) {
        draw_line({player_x, player_y}, {x, y}, 0xFF5E05FF);
      }

      const int npc_size =
          round_f32_to_i32(A.size / TILE_SIZE * PIXELS_IN_TILE / 2);

      pixel_t color = A.alive ? 0xFFFF00FF : 0xFF0000FF;

      draw_circle(x, y, npc_size, color);
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
      const bool facing_right = cos_f32(ray_angle) > 0 ? true : false;
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
      const bool facing_up = ray_angle > 0 ? false : true;

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
          // draw_2d_ray(horizontal_intersection_X,
          // horizontal_intersection_Y);
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
          floor_f32_to_i32(i * ray_column_width),
          (screen_h / 2) - (projection_h / 2),
          floor_f32_to_i32(ray_column_width),
          projection_h,
      };

      // draw_texture(texture, draw_position, wall_chunk);
      // draw_rect(draw_position, 0xFF0000FF);

      to_draw.emplace_back(depth, texture, draw_position, wall_chunk);

      // Floor section
    }
  }


  void prepare_to_draw_actor(const base_actor_t& actor)
  {
    // Calculate the angle that the player looks at the sprite
    const float dx = actor.position.x - player.position.x;
    const float dy = actor.position.y - player.position.y;

    const float theta = atan2_f32(dy, dx);

    // Calculate the difference between the player angle and the theta
    // angle. This will show how many rays the sprite is shifted from the
    // central ray

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

    const std::string& sprite_name = actor.sprite();
    const texture_t& texture = sprites[sprite_name];
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
      to_draw.emplace_back(norm_dist, texture, draw_rect);
    }
  }


  void prepare_to_draw_all_actors()
  {
    for (const auto& A : unanimated_actors) {
      prepare_to_draw_actor(A);
    }

    for (const auto& A : animated_actors) {
      prepare_to_draw_actor(A);
    }

    for (const auto& A : NPCs) {
      prepare_to_draw_actor(A);
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
    // Sort the to_draw
    std::sort(to_draw.begin(), to_draw.end(),
              [](const drawable_t& a, const drawable_t& b) {
                return a.depth > b.depth;
              });

    // Draw
    for (const drawable_t& D : to_draw) {
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
    const texture_t& texture = sprites[weapon.animation.sprite()];

    const int w = texture.w / 3;
    const int h = texture.h / 3;

    const rect_t r = {(screen_w - w) / 2, screen_h - h, w, h};
    draw_texture(texture, r);
  }


  void on_update(void*) override
  {
    to_draw.clear();

    /*------ Check events               ------*/
    check_events();

    /*------ Update the game            ------*/
    update_player();
    update_weapon();
    update_NPCs();

    /*------ Execute all the animation  ------*/
    animate();

    /*------ Draw                       ------*/

    // Ray cast
    ray_cast();
    prepare_to_draw_all_actors();

    // Draw
    draw_background();
    draw_drawables();
    draw_weapon();

    if (should_draw_map) {
      draw_2d_map();
      draw_2d_player();
    }

    // Print text
    draw_fps();
  }

  inline void log(const std::string& msg) override { LOG_E << msg << END_E; }
};


int main(int, char**)
{
  srand(time(0));
  gui_t gui(1024, 1024);

  do {
    reset_game = false;
    (void)gui.run();
  } while (reset_game);

  return EXIT_SUCCESS;
}