#pragma once
#include <Arduino.h>

template<class T, size_t N>
class StaticList {
private:
  T data[N];
  size_t _size = 0;

public:
  typedef T *iterator;
  typedef const T *const_iterator;

  iterator begin() {
    return &data[0];
  }

  iterator end() {
    return &data[_size];
  }

  const_iterator begin() const {
    return &data[0];
  }

  const_iterator end() const {
    return &data[_size];
  }
  void clear() {
    _size = 0;
  }
  bool contains(T v) {
    for (auto i : *this) {
      if (i == v) {
        return true;
      }
    }
    return false;
  }
  bool add(T v) {
    if (_size < N) {
      data[_size++] = v;
      return true;
    }
    return false;
  }
  size_t size() {
    return _size;
  }
  T &operator[](std::size_t index) {
    return data[index];
  }

  // Overloading operator[] as const for read-only access
  const T &operator[](std::size_t index) const {
    return data[index];
  }
};