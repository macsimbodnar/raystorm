#include <cassert>
#include "linked_list.hpp"
#include "log.hpp"

int linked_list_tests()
{
  list_t<int> list;
  auto e1 = list.insert(1);
  auto e2 = list.insert(2);

  (void)e1;
  (void)e2;

  assert(e1 == e2->prev);
  assert(e1->next == e2);

  for (auto I : list) {
    LOG_W << I << END_W;
  }

  LOG_I << list.size() << END_I;
  list.erase(e1);
  LOG_I << list.size() << END_I;
  assert(e2->prev == nullptr);
  assert(e2->next == nullptr);

  return 0;
}

int main()
{
  LOG_S << "Start testing..." << END_S;

  linked_list_tests();

  LOG_S << "End" << END_S;
  return 0;
}
