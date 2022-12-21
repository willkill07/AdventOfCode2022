#include <numeric>

#include "days/day04.hpp"
#include "owning_span.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day04, view) {
  owning_span<day04::range, day04::MAX_RANGES> ranges;
  for (usize off{0}; off < std::size(view);) {
    u32 a, b, c, d;
    off += parse<"\0-\1,\2-\3\n">(view.substr(off), a, b, c, d);
    ranges.push({{a, b}, {c, d}});
  }
  return ranges;
}

SOLVE_IMPL(Day04, Part2, ranges, part1_answer) {
  return std::accumulate(std::begin(ranges), std::end(ranges), 0u, [](u32 sum, day04::range const &r) {
    return sum + as<u32>(Part2 ? r.overlaps() : r.encloses());
  });
}

INSTANTIATE(Day04);

INSTANTIATE_TEST(Day04,
                 R"(
2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8
)"sv.substr(1),
                 2,
                 4)
