#pragma once

#include <algorithm>
#include <array>
#include <numeric>

#include "Types.hpp"

template <typename T, sz N>
consteval T
sum(std::array<T, N> const &arr) noexcept {
  return std::accumulate(std::begin(arr), std::end(arr), T{0});
}

template <typename T, sz N>
consteval std::array<T, N>
exclusive_sum(std::array<T, N> const &arr) noexcept {
  std::array<T, N> result;
  std::exclusive_scan(std::begin(arr), std::end(arr), std::begin(result), T{0});
  return result;
}

template <auto N>
void
static_for(auto &&body) noexcept {
  using T = decltype(N);
  [&]<T... Is>(std::integer_sequence<T, Is...>) {
    (body.template operator()<Is>(), ...);
  }
  (std::make_integer_sequence<T, N>{});
}
