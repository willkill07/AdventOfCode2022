#pragma once

#include <algorithm>
#include <array>
#include <numeric>



template <size_t N, typename T>
constexpr auto
all (T val) noexcept {
  std::array<T, N> res;
  res.fill(val);
  return res;
}

consteval bool
implies(bool a, bool b) {
  return !a or b;
}

template <typename T, std::size_t N>
consteval T
sum(std::array<T, N> const &arr) noexcept {
  return std::accumulate(std::begin(arr), std::end(arr), T{0});
}

template <typename T, std::size_t N>
consteval std::array<T, N>
exclusive_scan(std::array<T, N> const &arr) noexcept {
  std::array<T, N> result;
  std::exclusive_scan(std::begin(arr), std::end(arr), std::begin(result), T{0});
  return result;
}

template <auto N, typename Fn, typename... Args>
void
static_for(Fn &&body, Args &&...args) noexcept {
  using T = decltype(N);
  []<T... Is>(std::integer_sequence<T, Is...>, Fn && f, Args && ...a) {
    (f(std::integral_constant<T, Is>{}, std::forward<Args>(a)...), ...);
  }
  (std::make_integer_sequence<T, N>{}, std::forward<Fn>(body), std::forward<Args>(args)...);
}

template <auto N, typename Init, typename Fn, typename... Args>
auto
fold(Init init, Fn &&body, Args &&...args) noexcept {
  using T = decltype(N);
  return []<T... Is>(std::integer_sequence<T, Is...>, Init & acc, Fn && f, Args && ...a) {
    (f(acc, std::integral_constant<T, Is>{}, std::forward<Args>(a)...), ...);
    return acc;
  }
  (std::make_integer_sequence<T, N>{}, init, std::forward<Fn>(body), std::forward<Args>(args)...);
}