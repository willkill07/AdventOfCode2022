#include <algorithm>
#include <concepts>
#include <iterator>
#include <type_traits>

#include <cctype>

#include "Day03.hpp"

inline int
score(char c) noexcept {
  return (c - (::isupper(c) ? ('A' - 26) : 'a')) + 1;
}

template <std::random_access_iterator Iter1, std::random_access_iterator Iter2>
auto
unique_occuring(Iter1 a0, Iter1 a1, Iter2 b0, Iter2 b1) noexcept
  requires std::same_as<std::iter_value_t<Iter1>, std::iter_value_t<Iter2>>
{
  std::sort(a0, a1);
  std::sort(b0, b1);
  using T = std::common_type_t<std::iter_value_t<Iter1>, std::iter_value_t<Iter2>>;
  while (a0 != a1 and b0 != b1) {
    if (T const av{*a0}, bv{*b0}; av < bv) {
      ++a0;
    } else if (av > bv) {
      ++b0;
    } else {
      return av;
    }
  }
  return T{};
}

template <std::random_access_iterator Iter1, std::random_access_iterator Iter2, std::random_access_iterator Iter3>
auto
unique_occuring(Iter1 a0, Iter1 a1, Iter2 b0, Iter2 b1, Iter3 c0, Iter3 c1) noexcept
  requires std::same_as<std::iter_value_t<Iter1>, std::iter_value_t<Iter2>> and
           std::same_as<std::iter_value_t<Iter1>, std::iter_value_t<Iter3>>
{
  std::sort(a0, a1);
  std::sort(b0, b1);
  std::sort(c0, c1);
  using T = std::common_type_t<std::iter_value_t<Iter1>, std::iter_value_t<Iter2>, std::iter_value_t<Iter3>>;
  while (a0 != a1 and b0 != b1 and c0 != c1) {
    if (T const av{*a0}, bv{*b0}, cv{*c0}; av == bv and bv == cv) {
      return av;
    } else if (av < bv) { // a is the smallest
      while (++a0 != a1 and *a0 == av)
        ;
    } else if (bv < cv) { // b is the smallest
      while (++b0 != b1 and *b0 == bv)
        ;
    } else { // c is the smallest
      while (++c0 != c1 and *c0 == cv)
        ;
    }
  }
  return T{};
}

PARSE_IMPL(Day03, buffer) {
  std::vector<std::string_view> result;
  std::string_view view{buffer.data(), buffer.size()};
  for (sz off{0}; off < std::size(buffer); ) {
    auto const length = view.find_first_of('\n', off) - off;
    result.push_back(view.substr(off, length));
    off += length + 1;
  }
  return result;
}

PART1_IMPL(Day03, lines) {
  int answer{0};
  for (auto const &line : lines) {
    std::array<char, 64> A, B;
    sz const mid{line.size() / 2};
    std::string_view const lhs = line.substr(0, mid);
    std::string_view const rhs = line.substr(mid);
    answer += score(unique_occuring(std::begin(A),
                                    std::copy(std::begin(lhs), std::end(lhs), std::begin(A)),
                                    std::begin(B),
                                    std::copy(std::begin(rhs), std::end(rhs), std::begin(B))));
  }
  return answer;
}

PART2_IMPL(Day03, lines, part1_answer) {
  int answer{0};
  for (sz idx{0}; idx < std::size(lines); idx += 3) {
    std::array<char, 64> A, B, C;
    answer += score(unique_occuring(std::begin(A),
                                    std::copy(std::begin(lines[idx + 0]), std::end(lines[idx + 0]), std::begin(A)),
                                    std::begin(B),
                                    std::copy(std::begin(lines[idx + 1]), std::end(lines[idx + 1]), std::begin(B)),
                                    std::begin(C),
                                    std::copy(std::begin(lines[idx + 2]), std::end(lines[idx + 2]), std::begin(C))));
  }
  return answer;
}
