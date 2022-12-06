#include <algorithm>
#include <bit>
#include <numeric>

#include "Day06.hpp"
#include "Parsing.hpp"

PARSE_IMPL(Day06, view) {
  std::vector<i8> mapped(std::size(view));
  std::transform(std::begin(view), std::end(view), std::begin(mapped), [](char c) {
    return c - 'a';
  });
  return mapped;
}

SOLVE_IMPL(Day06, Part2, data, unused) {
  constexpr u32 const window{Part2 ? 14 : 4};
  for (auto i{std::begin(data) + window}; i < std::end(data); ++i) {
    u32 const set {std::accumulate(i - window, i, 0u, [](u32 set, i8 idx) {
      return set | (1 << idx);
    })};
    if (std::popcount(set) == window) [[unlikely]] {
      return std::distance(std::begin(data), i);
    }
  }
  return u32(-1);
}

INSTANTIATE(Day06);

TEST_CASE("Day 06") {
  using Day = Day06;
  constexpr auto const part1_expected = 7l;
  constexpr auto const part2_expected = 19l;
  constexpr std::string_view const sample_input = R"(mjqjpqmgbljsphdztnvjfqwrcgsmlb)"sv;

  Day day;
  auto const view = day.parse(sample_input);
  auto const part1_actual = day.solve<false>(view);
  auto const part2_actual = day.solve<true>(view, part1_actual);
  SUBCASE("Part 1") {
    (void)part1_actual;
    (void)part1_expected;
    CHECK(part1_actual == part1_expected);
  }
  SUBCASE("Part 2") {
    (void)part2_actual;
    (void)part2_expected;
    CHECK(part2_actual == part2_expected);
  }
}
