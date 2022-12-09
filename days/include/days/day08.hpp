#include <string_view>
#include <vector>

#include "days/day.hpp"

namespace day08 {
  struct result {
    std::vector<u32> visible;
    std::vector<std::string_view> grid;
    usize size;
  };
}

using Day08 = Day<8, day08::result, i64>;
