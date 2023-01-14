#pragma once
#include <stddef.h>

template <class T>
struct node_t
{
  T value;
  node_t* next;
  node_t* prev;

  node_t(const T v) : value(v), next(nullptr), prev(nullptr) {}
};


template <class T>
class list_t
{
private:
  node_t<T>* head;
  size_t _size;

public:
  class iterator
  {
  private:
    node_t<T>* current;

  public:
    iterator(node_t<T>* node) : current(node) {}

    T operator*() { return current->value; }

    void operator++() { current = current->next; }

    bool operator!=(const iterator& other) { return current != other.current; }
  };

  iterator begin() { return iterator(head); }

  iterator end() { return iterator(nullptr); }


  list_t() : head(nullptr), _size(0) {}

  inline node_t<T>* insert(const T e)
  {
    auto new_elem = new node_t(e);

    if (head == nullptr) {
      assert(new_elem != nullptr);
      assert(head == nullptr);

      head = new_elem;
    } else {
      assert(head != nullptr);

      auto last_elem = head;
      while (last_elem->next != nullptr) {
        last_elem = last_elem->next;
      }

      assert(new_elem->prev == nullptr);
      assert(new_elem->next == nullptr);
      assert(last_elem->next == nullptr);

      new_elem->prev = last_elem;
      last_elem->next = new_elem;
    }

    _size++;
    return new_elem;
  }

  inline void erase(node_t<T>* to_erase)
  {
    assert(to_erase != nullptr);

    auto prev = to_erase->prev;
    auto next = to_erase->next;

    if (prev == nullptr) {
      head = next;
    } else {
      prev->next = next;
    }

    if (next != nullptr) { next->prev = prev; }

    if (_size > 0) { _size--; }

    delete to_erase;
  }

  inline size_t size() const { return _size; }
};
