#pragma once

#include <algorithm>
#include <string_view>
#include <utility>

#include "Types.hpp"

template <size_t N>
struct fixed_string {

  char value[N];
  usize printable_size{0};
  usize size{N - 1};

  inline constexpr fixed_string(const char (&str)[N]) noexcept {
    std::copy_n(str, N, value);
    for (usize i{0}; i < size; i += length(i)) {
      ++printable_size;
    }
  }

  inline constexpr char operator()(usize index) const noexcept {
    return value[index];
  }

  inline constexpr std::string_view operator[](usize index) const noexcept {
    usize curr{0}, i{0};
    while (i < size) {
      if (usize const len = length(i); curr == index) {
        return {value + i, len};
      } else {
        ++curr;
        i += len;
      }
    }
    return {value + size, 0};
  }

private:
  inline constexpr usize length(usize offset) const noexcept {
    if (unsigned char const lb = static_cast<unsigned char>(value[offset]); (lb & 0x80) == 0) { // ascii
      return 1;
    } else if ((lb & 0xE0) == 0xC0) { // 110x xxxx
      return 2;
    } else if ((lb & 0xF0) == 0xE0) { // 1110 xxxx
      return 3;
    } else if ((lb & 0xF8) == 0xF0) { // 1111 0xxx
      return 4;
    }
    return 0;
  }
};

template <fixed_string Str, typename Fn, typename... Args>
[[gnu::always_inline]] inline void
iterate(Fn &&callback, Args &&...args) noexcept {
  []<usize... Ids>(std::integer_sequence<usize, Ids...>, auto &&cb, Args &&...a) {
    (cb.template operator()<Ids, Str(Ids)>(std::forward<Args>(a)...), ...);
  }
  (std::make_integer_sequence<usize, Str.size>{}, std::forward<Fn>(callback), std::forward<Args>(args)...);
}
