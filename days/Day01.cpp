#include <concepts>
#include <numeric>
#include <type_traits>
#include <utility>

#include "Day01.hpp"

template <std::unsigned_integral T>
[[gnu::always_inline]] T
str_to(std::string_view s) noexcept {
  return std::accumulate(std::begin(s) + 1, std::end(s), static_cast<T>(s.front() - '0'), [](T acc, char c) {
    return T{10} * acc + T(c - '0');
  });
}

template <std::signed_integral T>
[[gnu::always_inline]] T
str_to(std::string_view s) noexcept {
  bool const neg{s.front() == '-'};
  T const value {static_cast<T>(str_to<std::make_unsigned_t<T>>(s.substr(static_cast<sz>(neg))))};
  return neg ? -value : value;
}

PARSE_IMPL(Day01, buffer) {
  u32 sum{0};
  std::array<u32, 3> top3{0,0,0};
  std::string_view view{buffer.data(), buffer.size()};
  for (sz off{0}; off < std::size(view); ++off) {
    sz const len{view.substr(off).find_first_of('\n')};
    if (len > 0) {
      sum += str_to<u32>(view.substr(off, len));
      off += len;
    } else {
      if (sum > top3[2]) {
        if (sum > top3[1]) {
          top3[2] = top3[1];
          if (sum > top3[0]) {
            top3[1] = top3[0];
            top3[0] = sum;
          } else {
            top3[1] = sum;
          }
        } else {
          top3[2] = sum;
        }
      }
      sum = 0;
    }
  }
  return top3;
}

PART1_IMPL(Day01, data) {
  return data[0];
}

PART2_IMPL(Day01, data, part1_answer) {
  return data[0] + data[1] + data[2];
}
