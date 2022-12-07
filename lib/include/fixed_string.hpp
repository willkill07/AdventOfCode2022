#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string_view>
#include <utility>

#include "meta/utils.hpp"

template <size_t N>
struct fixed_string {
  char raw_data[N - 1];

  using value_type = char;
  using reference = char &;
  using const_reference = char const &;
  using pointer = char *;
  using const_pointer = char const *;
  using iterator = char *;
  using const_iterator = char const *;
  using difference_type = std::ptrdiff_t;

  inline constexpr fixed_string(const char (&str)[N]) noexcept {
    std::copy_n(str, N - 1, raw_data);
  }

  inline constexpr value_type operator[](std::size_t index) const noexcept {
    return raw_data[index];
  }

  inline constexpr reference operator[](std::size_t index) noexcept {
    return raw_data[index];
  }

  inline constexpr std::size_t size() const noexcept {
    return N - 1;
  }

  inline constexpr const_iterator begin() const noexcept {
    return raw_data;
  }

  inline constexpr const_iterator cbegin() const noexcept {
    return raw_data;
  }

  inline constexpr iterator begin() noexcept {
    return raw_data;
  }

  inline constexpr const_pointer data() const noexcept {
    return raw_data;
  }

  inline constexpr pointer data() noexcept {
    return raw_data;
  }

  inline constexpr const_iterator end() const noexcept {
    return begin() + size();
  }

  inline constexpr const_iterator cend() const noexcept {
    return begin() + size();
  }

  inline constexpr iterator end() noexcept {
    return begin() + size();
  }

  inline constexpr bool check(std::size_t idx) const noexcept {
    return idx < size();
  }
};

std::size_t constexpr u8_charpoint_size(char point) noexcept {
  if (unsigned char const lb = static_cast<unsigned char>(point); (lb & 0x80) == 0) { // ascii
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

template <fixed_string Str, std::size_t N>
inline constexpr std::array<std::size_t, N>
u8_charpoint_lengths() noexcept {
  std::array<std::size_t, N> lengths;
  for (std::size_t idx{0}, off{0}; idx < N; off += lengths[idx++]) {
    lengths[idx] = u8_charpoint_size(Str[off]);
  }
  return lengths;
}

template <fixed_string Str>
inline constexpr std::size_t
u8_length() noexcept {
  constexpr auto const N = Str.size();
  std::size_t off{0}, idx{0};
  while (off < N) {
    auto const len = u8_charpoint_size(Str[off]);
    off += len;
    ++idx;
  }
  return idx;
}

template <fixed_string Str>
struct fixed_u8string {
  static constexpr auto const raw = Str;
  static constexpr auto const length = u8_length<raw>();
  static constexpr auto const charpoint_lengths = u8_charpoint_lengths<raw, length>();
  static constexpr auto const offsets = exclusive_scan(charpoint_lengths);

  using value_type = std::string_view;

  struct iterator {

    std::size_t index;

    using value_type = std::string_view;
    using difference = std::ptrdiff_t;

    inline constexpr iterator &operator++() noexcept {
      ++index;
      return *this;
    }

    inline constexpr iterator operator++(int) noexcept {
      return iterator{index++};
    }

    inline constexpr iterator &operator--() noexcept {
      --index;
      return *this;
    }

    inline constexpr iterator operator--(int) noexcept {
      return iterator{index--};
    }

    inline constexpr iterator &operator+=(difference d) noexcept {
      index += d;
      return &this;
    }

    inline constexpr iterator &operator-=(difference d) noexcept {
      index -= d;
      return &this;
    }

    inline friend constexpr iterator operator+(iterator i, difference d) noexcept {
      return {i.index + d};
    }

    inline friend constexpr iterator operator+(difference d, iterator i) noexcept {
      return {i.index + d};
    }

    inline friend constexpr iterator operator-(iterator i, difference d) noexcept {
      return {i.index - d};
    }

    constexpr value_type operator*() const {
      return {Str.data() + offsets[index], charpoint_lengths[index]};
    }
  };

  inline constexpr value_type operator[](std::size_t index) const noexcept {
    return *iterator{index};
  }

  inline constexpr std::size_t size() const noexcept {
    return length;
  }

  inline constexpr iterator begin() const noexcept {
    return {&Str[0]};
  }

  inline constexpr iterator end() const noexcept {
    return begin() + size();
  }
};

inline constexpr bool
is_placeholder_char(char c, std::size_t Limit) noexcept {
  return static_cast<std::size_t>(c) < Limit;
}
