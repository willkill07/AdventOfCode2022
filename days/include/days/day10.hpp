#include <vector>

#include "days/day.hpp"

namespace day10 {

struct instruction {
  static constexpr inline instruction noop() noexcept {
    return instruction{};
  }

  static constexpr inline instruction addx(i32 v) noexcept {
    return instruction{v};
  }

  constexpr instruction(instruction &&) = default;
  constexpr instruction &operator=(instruction &&) = delete;

  constexpr instruction(instruction const &) = delete;
  constexpr instruction &operator=(instruction const &) = delete;

  constexpr inline i32 get_tick() const noexcept {
    return 1 + (value != 0);
  }

  constexpr inline i32 get_value() const noexcept {
    return value;
  }

  constexpr inline bool is_noop() const noexcept {
    return value == 0;
  }

  constexpr inline bool is_addx() const noexcept {
    return value != 0;
  }

private:
  constexpr inline instruction(i32 v = 0) noexcept
      : value(v) {
  }

  i32 value;
};
} // namespace day10

using Day10 = Day<10, std::vector<i32>, i32, std::string>;
