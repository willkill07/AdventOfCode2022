#include "Day01.hpp"
#include "Parsing.hpp"

PARSE_IMPL(Day01, view) {
  u32 sum{0};
  std::array<u32, 3> top3{0, 0, 0};
  for (usize off{0}; off < std::size(view);) {
    u32 val;
    usize const len = read<"\0\n">(view.substr(off), val);
    off += len;
    if (len > 1) {
      sum += val;
    } else {
      if (sum > top3[2]) {
        if (sum > top3[1]) {
          top3[2] = top3[1];
          if (sum > top3[0]) {
            top3[1] = top3[0];
            top3[0] = sum;
          } else {
            top3[1] = sum;
          }
        } else {
          top3[2] = sum;
        }
      }
      sum = 0;
    }
  }
  return top3;
}

PART1_IMPL(Day01, data) {
  return data[0];
}

PART2_IMPL(Day01, data, part1_answer) {
  return data[0] + data[1] + data[2];
}

TEST_CASE("Day 01") {
  using Day = Day01;
  constexpr auto const part1_expected = 24000;
  constexpr auto const part2_expected = 45000;
  constexpr std::string_view const sample_input = R"(1000
2000
3000

4000

5000
6000

7000
8000
9000

10000

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
