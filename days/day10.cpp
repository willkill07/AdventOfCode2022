#include <algorithm>
#include <doctest/doctest.h>

#include "days/day10.hpp"
#include "letter.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day10, view) {
  std::array<i32, 240> xvals;
  u32 idx{0};
  i32 X{1};
  for (usize off{0}; off < std::size(view);) {
    xvals[idx++] = X;
    if (view[std::exchange(off, off + 5)] == 'a') {
      i32 value;
      off += parse<"\0\n">(view.substr(off), value);
      xvals[idx++] = X;
      X += value;
    }
  }
  return xvals;
}

PART1_IMPL(Day10, xvals) {
  // array starts at 0, not 1
  return (xvals[19] + 20 * xvals[59] + 100 * xvals[99] + 140 * xvals[139] + 180 * xvals[179] + 220 * xvals[219]);
}

namespace {
// we know the output is going to be 8 chars always
// use a static buffer for this and always return the address
std::array<char, 8> screen;
} // namespace

PART2_IMPL(Day10, xvals, part1_answer) {
  std::array<letter, 8> letters;
  auto xv = std::begin(xvals);
  for (u8 row{0}; row < letter::height; ++row) {
    for (u8 idx{0}, column{0}; idx < 8; ++idx) {
      for (u8 col{0}; col < letter::width; ++col) {
        if (std::abs(column - *xv++) <= 1) {
          letters[idx].set_pixel(row, col);
        }
        ++column;
      }
    }
  }
  screen.fill(0);
  for (u32 i{0}; i < 8u; ++i) {
    screen[i] = letters[i].as<char>();
  }
  return {screen.data(), screen.size()};
}

INSTANTIATE_TEST(Day10,
                 R"(
addx 15
addx -11
addx 6
addx -3
addx 5
addx -1
addx -8
addx 13
addx 4
noop
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx -35
addx 1
addx 24
addx -19
addx 1
addx 16
addx -11
noop
noop
addx 21
addx -15
noop
noop
addx -3
addx 9
addx 1
addx -3
addx 8
addx 1
addx 5
noop
noop
noop
noop
noop
addx -36
noop
addx 1
addx 7
noop
noop
noop
addx 2
addx 6
noop
noop
noop
noop
noop
addx 1
noop
noop
addx 7
addx 1
noop
addx -13
addx 13
addx 7
noop
addx 1
addx -33
noop
noop
noop
addx 2
noop
noop
noop
addx 8
noop
addx -1
addx 2
addx 1
noop
addx 17
addx -9
addx 1
addx 1
addx -3
addx 11
noop
noop
addx 1
noop
addx 1
noop
noop
addx -13
addx -19
addx 1
addx 3
addx 26
addx -30
addx 12
addx -1
addx 3
addx 1
noop
noop
noop
addx -9
addx 18
addx 1
addx 2
noop
noop
addx 9
noop
noop
noop
addx -1
addx 2
addx -37
addx 1
addx 3
noop
addx 15
addx -21
addx 22
addx -6
addx 1
noop
addx 2
addx 1
noop
addx -10
noop
noop
addx 20
addx 1
addx 2
addx 2
addx -6
addx -11
noop
noop
noop
)"sv.substr(1),
                 13140,
                 "!@#$%^&*")
