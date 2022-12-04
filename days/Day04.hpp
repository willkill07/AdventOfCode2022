#include <vector>

#include "Day.hpp"

namespace day04 {
struct pair {
  u32 lo, hi;
};

struct range {
  pair a, b;
};
} // namespace day04

using Day04 = Day<4, std::vector<day04::range>, u32>;
