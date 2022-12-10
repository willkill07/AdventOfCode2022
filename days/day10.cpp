#include <algorithm>
#include <doctest/doctest.h>

#include "days/day10.hpp"
#include "letter.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day10, view) {
  std::vector<i32> values;
  values.reserve(2u * static_cast<u32>(std::count(std::begin(view), std::end(view), '\n')));
  for (usize off{0}; off < std::size(view);) {
    values.push_back(0);
    if (view[off] == 'a') {
      off += 5;
      i32 value;
      off += parse<"\0\n">(view.substr(off), value);
      values.push_back(value);
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
  std::string screen(8, ' ');
  for (u32 i{0}; i < 8u; ++i) {
    screen[i] = letters[i].as<char>();
  }
  return screen;
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
