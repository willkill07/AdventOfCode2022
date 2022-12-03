#include <algorithm>

#include <cctype>

#include "Day03.hpp"

PARSE_IMPL(Day03, buffer) {
  std::vector<std::string_view> result;
  auto const end = std::end(buffer);
  auto cursor = std::begin(buffer);
  while (cursor != end) {
    auto end_of_line = std::find(cursor, end, '\n');
    result.push_back(std::string_view{cursor, end_of_line});
    cursor = end_of_line + 1;
  }
  return result;
}

inline int
score(char c) noexcept {
  return (c - (::isupper(c) ? ('A' - 26) : 'a')) + 1;
}

PART1_IMPL(Day03, lines) {
  int answer{0};
  for (auto const &line : lines) {
    std::array<char, 64> A, B, C;
    sz const mid{line.size() / 2};
    std::string_view const lhs = line.substr(0, mid);
    std::string_view const rhs = line.substr(mid);
    auto const A1 = std::copy(std::begin(lhs), std::end(lhs), std::begin(A));
    std::sort(std::begin(A), A1);
    auto const B1 = std::copy(std::begin(rhs), std::end(rhs), std::begin(B));
    std::sort(std::begin(B), B1);
    std::set_intersection(std::begin(A), A1, std::begin(B), B1, std::begin(C));
    answer += score(C.front());
  }
  return answer;
}

PART2_IMPL(Day03, lines, part1_answer) {
  int answer{0};
  for (sz idx{0}; idx < std::size(lines); idx += 3) {
    std::array<char, 64> A, B, C;
    std::array<char, 32> T, U;
    auto const A1 = std::copy(std::begin(lines[idx + 0]), std::end(lines[idx + 0]), std::begin(A));
    std::sort(std::begin(A), A1);
    auto const B1 = std::copy(std::begin(lines[idx + 1]), std::end(lines[idx + 1]), std::begin(B));
    std::sort(std::begin(B), B1);
    auto const C1 = std::copy(std::begin(lines[idx + 2]), std::end(lines[idx + 2]), std::begin(C));
    std::sort(std::begin(C), C1);
    auto const T1 = std::set_intersection(std::begin(A), A1, std::begin(B), B1, std::begin(T));
    std::set_intersection(std::begin(C), C1, std::begin(T), T1, std::begin(U));
    answer += score(U.front());
  }
  return answer;
}
