#include <bit>
#include <numeric>

#include <cctype>

#include "Day03.hpp"

namespace {

[[gnu::always_inline, nodiscard]] inline u64
hash(std::string_view s) noexcept {
  return std::accumulate(std::begin(s), std::end(s), 0lu, [](u64 hash, char c) noexcept {
    auto const offset = static_cast<u32>(c - (::isupper(c) ? ('A' - 26) : 'a'));
    return hash | (1lu << offset);
  });
}

} // namespace

PARSE_IMPL(Day03, buffer) {
  std::vector<std::string_view> result;
  std::string_view view{buffer.data(), buffer.size()};
  for (sz off{0}; off < std::size(buffer);) {
    auto const length = view.find_first_of('\n', off) - off;
    result.push_back(view.substr(off, length));
    off += length + 1;
  }
  return result;
}

PART1_IMPL(Day03, lines) {
  int answer{0};
  for (auto const &line : lines) {
    sz const mid{line.size() / 2};
    u64 const l = hash(line.substr(0, mid));
    u64 const r = hash(line.substr(mid));
    answer += std::countr_zero(l & r) + 1;
  }
  return answer;
}

PART2_IMPL(Day03, lines, part1_answer) {
  int answer{0};
  auto iter{std::begin(lines)};
  while (iter != std::end(lines)) {
    u64 const a = hash(*iter++);
    u64 const b = hash(*iter++);
    u64 const c = hash(*iter++);
    answer += std::countr_zero(a & b & c) + 1;
  }
  return answer;
}
