#include <pixello.hpp>
#include "log.hpp"
#include "math.hpp"

constexpr int SCREEN_W = 1024;
constexpr int SCREEN_H = 768;
constexpr int TILE = 32;
constexpr float PLAYER_SPEED = 0.0000001;
constexpr float PLAYER_ROTATION_SPEED = 0.000000001;

// clang-format off
constexpr char minimap[SCREEN_H / TILE][SCREEN_W / TILE] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
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

struct pos_t
{
  int x;
  int y;
};

struct player_t
{
  pos_t pixel_pos;
  float angle;

  pos_t tile_pos() const
  {
    const pos_t tile = {pixel_pos.x / TILE, pixel_pos.y / TILE};
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
    const pixel_t yellow = 0xFFFF00FF;
    const pos_t tile = player.tile_pos();

    // Tile
    draw_rect({(tile.x * TILE) + 1, (tile.y * TILE), TILE - 2, TILE - 2}, gray);

    // Direction
    const point_t a = {player.pixel_pos.x, player.pixel_pos.y};
    const point_t b = {
        round_f32_to_i32(player.pixel_pos.x + 50 * cos_f32(player.angle)),
        round_f32_to_i32(player.pixel_pos.y + 50 * sin_f32(player.angle))};

    draw_line(a, b, yellow);

    // Body
    draw_circle(player.pixel_pos.x, player.pixel_pos.y, 10, green);

    // Print angle
    const texture_t angle = create_text("A: " + STR(player.angle), 0xFF0000FF);
    draw_texture(angle, 10, 10);
  }


  bool player_legal_move(int x, int y)
  {
    const pos_t tile = {x / TILE, y / TILE};

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
    draw_2d_player();
    draw_fps();
  }

  void on_init(void*) override
  {
    player.pixel_pos = {200, 200};
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