#pragma once
#include <Arduino.h>

template<class T, size_t N>
constexpr size_t arraySize(T (&)[N]) {
  return N;
}