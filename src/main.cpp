#include <pixello.hpp>
#include "log.hpp"

constexpr int SCREEN_W = 1024;
constexpr int SCREEN_H = 768;
constexpr int TILE = 32;

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
    const texture_t fps =
        create_text("FPS: " + std::to_string(FPS()), 0xFF0000FF);

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
    const pos_t tile = player.tile_pos();

    draw_rect({(tile.x * TILE) + 1, (tile.y * TILE), TILE - 2, TILE - 2}, gray);
    draw_circle(player.pixel_pos.x, player.pixel_pos.y, 10, green);
  }

  void check_events() {}
  void update() {}

  void draw()
  {
    clear_screen(0x00000000);
    draw_2d_map();
    draw_2d_player();
    draw_fps();
  }

  void on_init(void*) override { player.pixel_pos = {200, 200}; }

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