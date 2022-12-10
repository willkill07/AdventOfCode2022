#include <algorithm>
#include <doctest/doctest.h>

#include "days/day10.hpp"
#include "letter.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day10, view) {
  std::array<i8, 240> values;
  u32 idx{0};
  for (usize off{0}; off < std::size(view);) {
    values[idx++] = 0;
    if (view[off] == 'a') {
      off += 5;
      i32 value;
      off += parse<"\0\n">(view.substr(off), value);
      values[idx++] = static_cast<i8>(value);
    } else {
      off += 5;
    }
  }
  return values;
}

PART1_IMPL(Day10, diff) {
  i32 X{1}, score{0};
  u32 tick{0}; 
  while (tick < 20) {
    X += diff[tick++];
  }
  for (u32 times{0}; times < 6; ++times) {
    score += static_cast<i32>(tick) * X;
    for (u32 skip{0}; skip < 40; ++skip) {
      X += diff[tick++];
    }
  }
  return score;
}

namespace {
// we know the output is going to be 8 chars always
// use a static buffer for this and always return the address
std::array<char, 8> screen;
}

PART2_IMPL(Day10, diff, part1_answer) {
  std::array<letter, 8> letters;
  i32 valX{1};
  auto delta = std::begin(diff);
  for (u8 row{0}; row < letter::height; ++row) {
    for (u8 idx{0}, column{0}; idx < 8; ++idx) {
      for (u8 col{0}; col < letter::width; ++col, ++column) {
        if (std::abs(column - valX) <= 1) {
          letters[idx].set_pixel(row, col);
        }
        valX += *delta++;
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
