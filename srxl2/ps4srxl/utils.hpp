#pragma once
#include <Arduino.h>

template<class T, size_t N>
constexpr size_t arraySize(T (&)[N]) {
  return N;
}

int32_t map(int32_t v, int32_t from_min, int32_t from_max, int32_t to_min, int32_t to_max) {
  float delta = ((float)(v - from_min)) / (from_max - from_min);
  return to_min + delta * (to_max - to_min);
}

int32_t cap(int32_t v, int32_t min, int32_t max) {
  if (v < min) return min;
  if (v > max) return max;
  return v;
}