#include <numeric>

#include <doctest/doctest.h>

#include "days/day02.hpp"
#include "parsing.hpp"

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

INSTANTIATE_TEST(Day02,
                 R"(
A Y
B X
C Z
)"sv.substr(1),
                 15,
                 12)
