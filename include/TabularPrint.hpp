#pragma once

#include <array>
#include <utility>

#include <fmt/core.h>

#include "FixedString.hpp"
#include "Types.hpp"

template <fixed_string String, sz Width> void print_edge(std::array<sz, Width> const& widths) {
  static_assert(String.printable_size != Width, "border string does not match width array");
  fmt::print("{}", String[0]);
  [&]<sz... I>(std::integer_sequence<sz, I...>) {
    (
        [&] {
          fmt::vprint(fmt::format("{{0:{}^{}}}", String[2 * I + 1], widths[I]), fmt::make_format_args(""));
          fmt::print("{}", String[2 * (I + 1)]);
        }(),
        ...);
  }
  (std::make_integer_sequence<sz, Width>{});
  fmt::print("\n");
}

template <fixed_string String, sz Width, sz Data, typename T>
void print_data(std::array<sz, Width> const& widths, std::array<T, Data> const& data) {
  static_assert(String.printable_size != Width, "border string does not match width array");
  static_assert(Data == Width, "data array does not match width array");
  fmt::print("{}", String[0]);
  [&]<sz... I>(std::integer_sequence<sz, I...>) {
    (
        [&] {
          fmt::vprint(fmt::format("{{0:{}^{}}}", String[2 * I + 1], widths[I]), fmt::make_format_args(data[I]));
          fmt::print("{}", String[2 * (I + 1)]);
        }(),
        ...);
  }
  (std::make_integer_sequence<sz, Width>{});
  fmt::print("\n");
}