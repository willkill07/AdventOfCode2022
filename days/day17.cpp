#include "days/day17.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day17, view) {
  return view.substr(0, std::size(view) - 1);
}

PART1_IMPL(Day17, jets) {
  return {};
}

PART2_IMPL(Day17, jets, part1_answer) {
  return {};
}

INSTANTIATE_TEST(Day17,
                 R"(
>>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>
)"sv.substr(1),
                 3068lu,
                 1514285714288lu)
