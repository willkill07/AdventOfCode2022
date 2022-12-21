#include <algorithm>
#include <bit>
#include <numeric>

#include "days/day06.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day06, view) {
  typename day06::buffer_t mapped(as<u32>(std::size(view)) - 1);
  std::transform(std::begin(view), std::end(view) - 1, std::begin(mapped), [](char c) {
    return c - 'a';
  });
  return mapped;
}

SOLVE_IMPL(Day06, Part2, data, part1_answer) {
  constexpr u32 const window{Part2 ? 14 : 4};
  for (auto i{std::begin(data) + part1_answer.value_or(window)}; i < std::end(data); ++i) {
    u32 const set{std::accumulate(i - window, i, 0u, [](u32 num, i8 idx) {
      return num | (1 << idx);
    })};
    if (std::popcount(set) == window) [[unlikely]] {
      return std::distance(std::begin(data), i);
    }
  }
  return u32(-1);
}

INSTANTIATE(Day06);

INSTANTIATE_TEST(Day06,
                 R"(
mjqjpqmgbljsphdztnvjfqwrcgsmlb
)"sv.substr(1),
                 7L,
                 19L)
