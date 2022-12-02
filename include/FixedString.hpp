#pragma once

#include <algorithm>
#include <string_view>

#include "Types.hpp"

template <size_t N>
struct fixed_string {

  char value[N];
  sz printable_size{0};
  sz size{N - 1};

  inline constexpr fixed_string(const char (&str)[N]) {
    std::copy_n(str, N, value);
    for (sz i{0}; i < size; i += length(i)) {
      ++printable_size;
    }
  }

  inline constexpr std::string_view operator[](sz index) const {
    sz curr{0}, i{0};
    while (i < size) {
      if (sz const len = length(i); curr == index) {
        return {value + i, len};
      } else {
        ++curr;
        i += len;
      }
    }
    return {value + size, 0};
  }

private:
  inline constexpr sz length(sz offset) const noexcept {
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