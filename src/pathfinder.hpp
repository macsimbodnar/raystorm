#pragma once

#include <array>
#include <cassert>
#include <pixello.hpp>
#include <queue>
#include <unordered_map>
#include "log.hpp"
#include "math.hpp"
#include "memory"


namespace pathfinder
{

struct node_t
{
  int x;
  int y;
  std::shared_ptr<node_t> parent;

  node_t() : x(0), y(0) {}
  node_t(const int _x, const int _y) : x(_x), y(_y) {}
  node_t(const int _x, const int _y, const std::shared_ptr<node_t>& _p)
      : x(_x), y(_y), parent(_p)
  {}
};


bool operator==(const node_t& lhs, const node_t& rhs)
{
  bool result = (lhs.x == rhs.x && lhs.y == rhs.y);
  return result;
}


bool operator!=(const node_t& lhs, const node_t& rhs)
{
  bool result = (lhs.x != rhs.x || lhs.y != rhs.y);
  return result;
}


inline std::vector<std::shared_ptr<node_t>> get_neighbors(
    const std::shared_ptr<node_t>& node,
    const std::vector<std::vector<char>>& map,
    const int w,
    const int h)
{
  assert((int)map.size() == h);
  assert(map.size() > 0);
  assert((int)map[0].size() == w);

  std::vector<std::shared_ptr<node_t>> neighbors;
  neighbors.reserve(9);

  /**
   * MOVEMENTS
   *
   * [-1,-1][ 0,-1][ 1,-1]
   * [-1, 0][ 0, 0][ 1, 0]
   * [-1, 1][ 0, 1][ 1, 1]
   *
   */
  static const std::array<point_t, 4> directions = {
      point_t(0, -1), point_t(-1, 0), point_t(1, 0), point_t(0, 1)};

  for (const auto& I : directions) {
    const int x = node.get()->x + I.x;
    const int y = node.get()->y + I.y;

    if (x >= 0 && x < w && y >= 0 && y < h && map[y][x] == 0) {
      std::shared_ptr<node_t> neighbor = std::make_shared<node_t>(x, y, node);
      neighbors.push_back(neighbor);
    }
  }

  return neighbors;
}


inline const char* get_cell_pointer(const std::vector<std::vector<char>>& map,
                                    const node_t& node)
{
  const char* ptr = &(map[node.y][node.x]);
  return ptr;
}


inline std::vector<node_t> BFS(const node_t& start,
                               const node_t& end,
                               const std::vector<std::vector<char>>& map,
                               const int w,
                               const int h)
{
  std::queue<std::shared_ptr<node_t>> q;
  std::unordered_map<const char*, bool> visited;

  q.push(std::make_shared<node_t>(start));
  visited[get_cell_pointer(map, start)] = true;

  while (!q.empty()) {
    const auto current = q.front();
    q.pop();

    if (*(current.get()) == end) {
      std::vector<node_t> path;

      auto current_node = current;

      while (current_node != nullptr) {
        path.push_back(*current_node);
        current_node = current_node->parent;
      }

      return path;
    }

    for (auto neighbor : get_neighbors(current, map, w, h)) {
      const char* ptr = get_cell_pointer(map, *(neighbor.get()));
      if (!visited[ptr]) {
        q.push(neighbor);
        visited[ptr] = true;
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

  node_t f = {from.x, from.y};
  node_t t = {to.x, to.y};
  std::vector<node_t> full_path = BFS(f, t, map, w, h);

  const size_t size = full_path.size();
  if (size < 2) {
    next_position = from;
  } else {
    const auto& pos = full_path[size - 2];
    next_position = {pos.x, pos.y};
  }

  return next_position;
}
}  // namespace pathfinder
