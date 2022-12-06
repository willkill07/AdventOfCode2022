#include <numeric>

#include "Day02.hpp"
#include "Parsing.hpp"

template <bool Part2>
consteval day02::lookup_table_t
make_cost_table() noexcept {
  if constexpr (Part2) {
    return {3, 4, 8, 1, 5, 9, 2, 6, 7};
  } else {
    return {4, 8, 3, 1, 5, 9, 7, 2, 6};
  }
}

PARSE_IMPL(Day02, view) {
  day02::lookup_table_t counts{};
  for (usize idx{0}; idx < std::size(view); idx += 4) {
    ++counts[static_cast<u32>(view[idx] - 'A') * day02::options + static_cast<u32>(view[idx + 2] - 'X')];
  }
  return counts;
}

SOLVE_IMPL(Day02, Part2, parse_result, part1_answer) {
  return std::inner_product(std::begin(parse_result), std::end(parse_result), std::begin(make_cost_table<Part2>()), 0);
}

INSTANTIATE(Day02);

TEST_CASE("Day 02") {
  using Day = Day02;
  constexpr auto const part1_expected = 15;
  constexpr auto const part2_expected = 12;
  constexpr std::string_view const sample_input = R"(A Y
B X
C Z
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
