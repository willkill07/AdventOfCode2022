#include "Day04.hpp"
#include "Parsing.hpp"

PARSE_IMPL(Day04, buffer) {
  std::vector<day04::range> ranges;
  std::string_view view{buffer.data(), buffer.size()};
  for (sz off{0}; off < std::size(view);) {
    u32 a, b, c, d;
    off += read<"\0-\1,\2-\3\n">(view.substr(off), a, b, c, d);
    ranges.push_back({{a, b}, {c, d}});
  }
  return ranges;
}

SOLVE_IMPL(Day04, Part2, ranges, part1_answer) {
  u32 sum{0};
  for (auto const r : ranges) {
    sum += static_cast<u32>(Part2 ? r.overlaps() : r.encloses());
  }
  return sum;
}

INSTANTIATE(Day04);