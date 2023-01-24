#pragma once
#include <cassert>
#include <pixello.hpp>
#include <queue>
#include <unordered_map>
#include "log.hpp"
#include "math.hpp"


struct node_t
{
  int x;
  int y;
  node_t* parent;

  node_t() : x(0), y(0), parent(nullptr) {}
  node_t(const int _x, const int _y, node_t* _p) : x(_x), y(_y), parent(_p) {}
};


inline std::vector<node_t> get_neighbors(
    node_t* node,
    const std::vector<std::vector<char>>& map,
    const int w,
    const int h)
{
  assert((int)map.size() == h);
  assert(map.size() > 0);
  assert((int)map[0].size() == h);

  std::vector<node_t> neighbors;
  neighbors.reserve(9);

  /**
   * MOVEMENTS
   *
   * [-1,-1][ 0,-1][ 1,-1]
   * [-1, 0][ 0, 0][ 1, 0]
   * [-1, 1][ 0, 1][ 1, 1]
   *
   */

  for (int i = -1; i < 1; ++i) {
    for (int j = -1; j < 1; ++j) {
      if (i == 0 && j == 0) { continue; }

      const int x = node->x + i;
      const int y = node->y + j;

      if (x >= 0 && x < w && y >= 0 && y < h && map[x][y] == 0) {
        const node_t neighbor = {x, y, node};
        neighbors.push_back(std::move(neighbor));
      }
    }
  }

  return neighbors;
}


inline std::vector<node_t> BFS(node_t& start,
                               node_t& end,
                               const std::vector<std::vector<char>>& map,
                               const int w,
                               const int h)
{
  std::queue<node_t> q;
  std::unordered_map<int, bool> visited;

  q.push(start);
  visited[(start.y * w) + start.x] = true;

  while (!q.empty()) {
    node_t current = q.front();
    q.pop();

    if (current.x == end.x && current.y == end.y) {
      std::vector<node_t> path;

      node_t* current_node = &current;

      while (current_node != nullptr) {
        path.push_back(*current_node);
        current_node = current_node->parent;
      }

      return path;
    }

    for (auto& neighbor : get_neighbors(&current, map, w, h)) {
      const int id = (neighbor.y * w) + neighbor.x;
      if (!visited[id]) {
        q.push(neighbor);
        visited[id] = true;
      }
    }
  }

  return {};
}


inline point_t find_path(const point_t& from,
                         const point_t& to,
                         const std::vector<std::vector<char>>& map,
                         const int w,
                         const int h)
{
  point_t next_position;

  node_t f = {from.x, from.y, nullptr};
  node_t t = {to.x, to.y, nullptr};
  std::vector<node_t> full_path = BFS(f, t, map, w, h);

  LOG_I << "PATH(" << full_path.size() << "): ";
  for (const auto& I : full_path) {
    LOG_I << "[" << I.x << "," << I.y << "] -> ";
  }
  LOG_I << "END" << END_I;

  return next_position;
}