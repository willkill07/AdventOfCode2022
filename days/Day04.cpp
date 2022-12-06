#include <numeric>

#include "Day04.hpp"
#include "Parsing.hpp"

PARSE_IMPL(Day04, view) {
  std::vector<day04::range> ranges;
  for (usize off{0}; off < std::size(view);) {
    u32 a, b, c, d;
    off += read<"\0-\1,\2-\3\n">(view.substr(off), a, b, c, d);
    ranges.push_back({{a, b}, {c, d}});
  }
  return ranges;
}

SOLVE_IMPL(Day04, Part2, ranges, part1_answer) {
  return std::accumulate(std::begin(ranges), std::end(ranges), 0u, [&] (u32 sum, day04::range const& r) {
    return sum + static_cast<u32>(Part2 ? r.overlaps() : r.encloses());
  });
}

INSTANTIATE(Day04);

TEST_CASE("Day 04") {
  using Day = Day04;
  constexpr auto const part1_expected = 2;
  constexpr auto const part2_expected = 4;
  constexpr std::string_view const sample_input = R"(2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8
)"sv;

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
