#include <concepts>
#include <optional>
#include <span>
#include <utility>

#include "Day01.hpp"

class parse_state {
  int curr_calories{0};
  int curr_sum{0};
  bool saw_space{false};

public:
  void advance(char curr,
               std::invocable<int> auto &&on_value_callback) noexcept {
    bool const is_digit{'0' <= curr and curr <= '9'};
    if (is_digit) [[likely]] {
      curr_calories = (10 * curr_calories) + (curr - '0');
    } else if (saw_space) [[unlikely]] {
      on_value_callback(std::exchange(curr_sum, 0));
    } else {
      curr_sum += std::exchange(curr_calories, 0);
    }
    saw_space = not is_digit;
  }
};

PARSE_IMPL(Day01, buffer) {
  parse_state state;
  std::array<int, 3> top3{0, 0, 0};
  for (char const curr : buffer) {
    state.advance(curr, [&](int value) {
      if (value > top3[0]) {
        std::swap(value, top3[0]);
      }
      if (value > top3[1]) {
        std::swap(value, top3[1]);
      }
      if (value > top3[2]) {
        top3[2] = value;
      }
    });
  }
  return top3;
}

PART1_IMPL(Day01, data) { return data[2]; }

PART2_IMPL(Day01, data, part1_answer) { return data[0] + data[1] + data[2]; }
