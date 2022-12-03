#include <algorithm>

#include <cctype>
#include <fmt/ranges.h>
#include <string_view>

#include "Day.hpp"
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


PART1_IMPL(Day03, parse_result) {
  int answer{0};
  for (auto const &line : parse_result) {
    std::array<char, 64> L, R, T;
    auto const lhs = line.substr(0, line.size() / 2);
    auto const rhs = line.substr(line.size() / 2);
    auto end_l = std::copy(std::begin(lhs), std::end(lhs), std::begin(L));
    std::sort(std::begin(L), end_l);
    auto end_r = std::copy(std::begin(rhs), std::end(rhs), std::begin(R));
    std::sort(std::begin(R), end_r);
    std::set_intersection(std::begin(L), end_l, std::begin(R), end_r, std::begin(T));
    if (char const out{*std::begin(T)}; ::isupper(out)) {
      answer += 26 + (out - 'A') + 1;
    } else {
      answer += (out - 'a') + 1;
    }
  }
  return answer;
}

PART2_IMPL(Day03, parse_result, part1_answer) {
  int answer{0};
  for (sz idx{0}; idx < std::size(parse_result); idx += 3) {
    std::array<char, 64> L, R, T;
    auto end_l = std::copy(std::begin(parse_result[idx]), std::end(parse_result[idx]), std::begin(L));
    std::sort(std::begin(L), end_l);
    auto end_r = std::copy(std::begin(parse_result[idx + 1]), std::end(parse_result[idx + 1]), std::begin(R));
    std::sort(std::begin(R), end_r);
    auto const end_t = std::set_intersection(std::begin(L), end_l, std::begin(R), end_r, std::begin(T));
    end_l = std::copy(std::begin(parse_result[idx + 2]), std::end(parse_result[idx + 2]), std::begin(L));
    std::sort(std::begin(L), end_l);
    std::set_intersection(std::begin(L), end_l, std::begin(T), end_t, std::begin(R));
    if (char const out{*std::begin(R)}; ::isupper(out)) {
      answer += 26 + (out - 'A') + 1;
    } else {
      answer += (out - 'a') + 1;
    }
  }
  return answer;
}
