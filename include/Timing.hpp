#pragma once

#include <chrono>

using clock_type = std::chrono::steady_clock;
using time_point = clock_type::time_point const;

inline double time_in_us(auto start, auto stop) noexcept {
  return std::chrono::duration<double, std::micro>(stop - start).count();
}

struct timing_data {
  double parsing{0.0};
  double part1{0.0};
  double part2{0.0};

  inline double total() const noexcept { return parsing + part1 + part2; }

  inline timing_data &operator+=(timing_data const &other) noexcept {
    parsing += other.parsing;
    part1 += other.part1;
    part2 += other.part2;
    return *this;
  }
};