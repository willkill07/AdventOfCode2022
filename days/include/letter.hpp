#pragma once

#include <concepts>

#include <fmt/core.h>

#include "types.hpp"

struct letter {
  static constexpr inline u8 width{5};
  static constexpr inline u8 height{6};

  constexpr inline void set_pixel(u8 r, u8 c) noexcept {
    value |= get_bit_mask(r, c);
  }

  constexpr inline void unset_pixel(u8 r, u8 c) noexcept {
    value &= ~get_bit_mask(r, c);
  }

  constexpr inline void toggle_pixel(u8 r, u8 c) noexcept {
    value ^= get_bit_mask(r, c);
  }

  constexpr inline void print() const noexcept {
    for (u8 r{0}; r < height; ++r) {
      for (u8 c{0}; c < width; ++c) {
        if (value & get_bit_mask(r, c)) {
          fmt::print("#");
        } else {
          fmt::print(".");
        }
      }
      fmt::print("\n");
    }
  }

  template <typename T>
  constexpr inline T as() const noexcept {
    if constexpr (std::same_as<T, char>) {
      switch (value) {
      // test pattern
      case 0x3fffbcf3u:
        return '!';
      case 0x061061d9u:
        return '@';
      case 0x21cf8f8cu:
        return '#';
      case 0x3e707b06u:
        return '$';
      case 0x030fc233u:
        return '%';
      case 0x31f01c79u:
        return '^';
      case 0x3e0ff0ecu:
        return '&';
      case 0x01e005c6u:
        return '*';
      // letters
      case 0x1297a526u:
        return 'A';
      case 0x0e949d27u:
        return 'B';
      case 0x0c908526u:
        return 'C';
      case 0x1e109c2fu:
        return 'E';
      case 0x02109c2fu:
        return 'F';
      case 0x1c968526u:
        return 'G';
      case 0x1294bd29u:
        return 'H';
      case 0x0c94210cu:
        return 'J';
      case 0x12528ca9u:
        return 'K';
      case 0x1e108421u:
        return 'L';
      case 0x0213a527u:
        return 'P';
      case 0x1253a527u:
        return 'R';
      case 0x0c94a529u:
        return 'U';
      case 0x1e11110fu:
        return 'Z';
      // unknown
      case 0x0:
        return ' ';
      default:
        return '?';
      }
    } else {
      return static_cast<T>(value);
    }
  }

private:
  constexpr inline u32 get_bit_mask(u8 r, u8 c) const noexcept {
    return 1u << (r * width + c);
  }

  u32 value{0};
};
