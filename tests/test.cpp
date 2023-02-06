#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <memory>
#include <vector>
#include "linked_list.hpp"
#include "log.hpp"
#include "pathfinder.hpp"


TEST_SUITE("Test linked list")
{
  TEST_CASE("Simple list usage")
  {
    list_t<int> list;
    auto e1 = list.insert(1);
    auto e2 = list.insert(2);

    REQUIRE_EQ(e1, e2->prev);
    REQUIRE_EQ(e1->next, e2);

    for (auto I : list) {
      LOG_W << I << END_W;
    }

    LOG_I << list.size() << END_I;
    list.erase(e1);
    LOG_I << list.size() << END_I;
    REQUIRE_EQ(e2->prev, nullptr);
    REQUIRE_EQ(e2->next, nullptr);
  }
}


inline void print_nodes(const std::vector<pathfinder::node_t>& nodes)
{
  LOG_I << "NODES(" << nodes.size() << ") ";
  for (const auto& I : nodes) {
    LOG_I << "-> [" << I.x << "," << I.y << "] ";
  }
  LOG_I << END_I;
}


TEST_SUITE("Test pathfinder")
{
  static const int w = 8;
  static const int h = 8;
  // clang-format off
  static const std::vector<std::vector<char>> map = {
    // 0,1,2,3,4,5,6,7
      {1,1,1,1,1,1,1,1}, // 0
      {1,0,1,1,1,1,1,1}, // 1
      {1,1,0,1,1,1,1,1}, // 2
      {1,1,1,0,1,1,1,1}, // 3
      {1,0,0,0,0,0,0,1}, // 4
      {1,0,0,0,0,1,0,1}, // 5
      {1,0,0,0,0,1,0,1}, // 6
      {1,1,1,1,1,1,1,1}, // 7
  };
  // clang-format on

  TEST_CASE("Testing node")
  {
    struct test_case_t
    {
      point_t test_cell;
      std::vector<point_t> neighbors;
    };

    // clang-format off
    const static std::vector<test_case_t> test_cases = {
      {{6,6}, {{6, 5}}},
      {{2,5}, {{1, 5}, {2, 4}, {2, 6}, {3, 5}}},
      // {{2,5}, {{1, 5}, {2, 4}, {2, 6}, {3, 5}, {1, 4}, {3, 4}, {1, 6}, {3, 6}}},
    };
    // clang-format on

    for (const auto& test_case : test_cases) {
      std::shared_ptr<pathfinder::node_t> node =
          std::make_shared<pathfinder::node_t>(test_case.test_cell.x,
                                               test_case.test_cell.y);

      const auto neighbors = pathfinder::get_neighbors(node, map, w, h);

      REQUIRE_EQ(neighbors.size(), test_case.neighbors.size());

      for (const auto& expected : test_case.neighbors) {
        bool found = false;
        for (const auto& result : neighbors) {
          if (result->x == expected.x && result->y == expected.y) {
            found = true;
            break;
          }
        }

        REQUIRE(found);
      }
    }
  }

  TEST_CASE("Testing BFS")
  {
    const pathfinder::node_t start = {1, 1};
    const pathfinder::node_t end = {1, 4};
    const auto result = pathfinder::BFS(start, end, map, w, h);

    print_nodes(result);
  }

  TEST_CASE("Testing the path finding")
  {
    const point_t next_step = pathfinder::find_path({6, 4}, {6, 6}, map, 8, 8);
    REQUIRE_EQ(next_step.x, 6);
    REQUIRE_EQ(next_step.y, 5);
  }
}
