#include <bit>
#include <numeric>

#include "Day03.hpp"
#include "Parsing.hpp"

namespace {

[[gnu::always_inline, nodiscard]] inline u64
hash(std::string_view s) noexcept {
  return std::accumulate(std::begin(s), std::end(s), 0lu, [](u64 hash, char c) noexcept {
    auto const offset = static_cast<u32>(c - (c >= 'a' ? 'a' : 'A' - 26));
    return hash | (1lu << offset);
  });
}

} // namespace

PARSE_IMPL(Day03, view) {
  std::vector<std::string_view> result;
  for (usize off{0}; off < std::size(view); ) {
    std::string_view line;
    off += read<"\0\n">(view.substr(off), line);
    result.push_back(line);
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
  u64 set{-1lu};
  for (auto line : lines) {
    set &= hash(line);
    if (++count == 3) {
      answer += std::countr_zero(set) + 1;
      count = 0;
      set = -1lu;
    }
  }
  return answer;
}

TEST_CASE("Day 03") {
  using Day = Day03;
  constexpr auto const part1_expected = 157;
  constexpr auto const part2_expected = 70;
  constexpr std::string_view const sample_input = R"(vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw
)"sv;

  Day day;
  auto const view = day.parse(sample_input);
  auto const part1_actual = day.solve<false>(view);
  auto const part2_actual = day.solve<true>(view, part1_actual);
  SUBCASE("Part 1") {
    (void)part1_actual;
    (void)part1_expected;
    CHECK(part1_actual == part1_expected);
  }
  SUBCASE("Part 2") {
    (void)part2_actual;
    (void)part2_expected;
    CHECK(part2_actual == part2_expected);
  }
}
