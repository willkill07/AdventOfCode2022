#pragma once

#include <bit>

#include <cmath>

#include "types.hpp"

struct point2d {
  i32 x{0}, y{0};

  constexpr inline point2d() noexcept = default;

  constexpr inline point2d(i32 x_, i32 y_) noexcept
      : x{x_},
        y{y_} {
  }

  constexpr inline point2d &operator+=(point2d const &o) noexcept {
    x += o.x;
    y += o.y;
    return *this;
  }

  constexpr inline point2d &operator-=(point2d const &o) noexcept {
    x -= o.x;
    y -= o.y;
    return *this;
  }

  constexpr inline point2d &operator*=(i32 v) noexcept {
    x *= v;
    y *= v;
    return *this;
  }

  constexpr inline point2d &operator/=(i32 v) noexcept {
    x /= v;
    y /= v;
    return *this;
  }

  constexpr inline point2d operator+(point2d const &o) const noexcept {
    return point2d{x + o.x, y + o.y};
  }

  constexpr inline point2d operator-(point2d const &o) const noexcept {
    return point2d{x - o.x, y - o.y};
  }

  constexpr inline point2d operator*(i32 v) const noexcept {
    return point2d{x * v, y * v};
  }

  constexpr inline point2d operator/(i32 v) const noexcept {
    return point2d{x / v, y / v};
  }

  friend constexpr inline auto operator<=>(point2d const &, point2d const &) = default;

  constexpr usize hash() const noexcept {
    return std::bit_cast<usize>(*this);
  }

  static constexpr inline point2d right() noexcept {
    return {1, 0};
  }

  static constexpr inline point2d up() noexcept {
    return {0, 1};
  }

  static constexpr inline point2d left() noexcept {
    return {-1, 0};
  }

  static constexpr inline point2d down() noexcept {
    return {0, -1};
  }

  static constexpr inline point2d origin() noexcept {
    return {0, 0};
  }
};

struct point2d_hash {
  constexpr inline usize operator()(point2d const &p) const noexcept {
    return p.hash();
  }
};
