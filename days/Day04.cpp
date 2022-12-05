#include <numeric>

#include "Day04.hpp"
#include "Parsing.hpp"

PARSE_IMPL(Day04, view) {
  std::vector<day04::range> ranges;
  for (sz off{0}; off < std::size(view);) {
    u32 a, b, c, d;
    off += read<"\0-\1,\2-\3\n">(view.substr(off), a, b, c, d);
    ranges.push_back({{a, b}, {c, d}});
  }
  return ranges;
}

SOLVE_IMPL(Day04, Part2, ranges, part1_answer) {
  return std::accumulate(std::begin(ranges), std::end(ranges), 0u, [&] (u32 sum, day04::range const& r) {
    return sum + static_cast<u32>(Part2 ? r.overlaps() : r.encloses());
  });
}

INSTANTIATE(Day04);
