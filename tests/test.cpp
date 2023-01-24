#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include "linked_list.hpp"
#include "log.hpp"

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

TEST_SUITE("Test pathfinder") {}
