#include <bit>
#include <numeric>

#include "days/day03.hpp"
#include "parsing.hpp"

namespace {

[[gnu::always_inline, nodiscard]] inline u64
hash(std::string_view s) noexcept {
  return std::accumulate(std::begin(s), std::end(s), 0LU, [](u64 hash, char c) noexcept {
    auto const offset = as<u32>(c - (c >= 'a' ? 'a' : 'A' - 26));
    return hash | (1LU << offset);
  });
}

} // namespace

PARSE_IMPL(Day03, view) {
  day03::list_t result;
  for (usize off{0}; off < std::size(view);) {
    usize const sz{view.find_first_of('\n', off) - off};
    result.push(view.substr(off, sz));
    off += sz + 1;
  }
  return result;
}

PART1_IMPL(Day03, lines) {
  return std::accumulate(std::begin(lines), std::end(lines), 0, [](int acc, std::string_view line) {
    usize const mid{line.size() / 2};
    u64 const l = hash(line.substr(0, mid));
    u64 const r = hash(line.substr(mid));
    return acc + std::countr_zero(l & r) + 1;
  });
}

PART2_IMPL(Day03, lines, part1_answer) {
  int answer{0};
  int count{0};
  u64 set{0xFFFFFFFFFFFFFFFFLU};
  for (auto line : lines) {
    set &= hash(line);
    if (++count == 3) {
      answer += std::countr_zero(set) + 1;
      count = 0;
      set = 0xFFFFFFFFFFFFFFFFLU;
    }
  }
  return answer;
}

INSTANTIATE_TEST(Day03,
                 R"(
vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw
)"sv.substr(1),
                 157,
                 70)
