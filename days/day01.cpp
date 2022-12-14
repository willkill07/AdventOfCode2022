#include "days/day01.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day01, view) {
  u32 sum{0};
  std::array<u32, 3> top3{0, 0, 0};
  for (usize off{0}; off < std::size(view);) {
    u32 val;
    usize const len = parse<"\0\n">(view.substr(off), val);
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

INSTANTIATE_TEST(Day01,
                 R"(
1000
2000
3000

4000

5000
6000

7000
8000
9000

10000

)"sv.substr(1),
                 24000,
                 45000)
