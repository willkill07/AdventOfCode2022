#pragma once

#include <concepts>
#include <optional>
#include <string>
#include <string_view>

#include <fmt/core.h>

#include "Types.hpp"

struct run_options {
  constexpr inline static u32 default_precision{2};
  std::optional<u32> precision;
  bool colorize{true};
  bool timing{true};
  bool part2{true};
  bool part1{true};
  bool answers{true};
  bool mask{false};
  std::optional<u32> single{std::nullopt};
  std::optional<u32> benchmark{std::nullopt};

  [[nodiscard]] inline std::string format(std::integral auto value) const noexcept {
    return fmt::format("{0}", value);
  }

  [[nodiscard]] inline std::string format(std::floating_point auto value) const noexcept {
    return fmt::format("{0:.{1}f}", value, precision.value_or(run_options::default_precision));
  }

  [[nodiscard]] inline std::string format(std::string_view view) const noexcept {
    return std::string{view};
  }

  template <typename T>
  [[nodiscard]] inline std::string format_answer(T &&value) const noexcept {
    if (std::string result{format(std::forward<T>(value))}; mask) {
      std::fill(std::begin(result), std::end(result), 'X');
      return result;
    } else {
      return result;
    }
  }
  
  [[nodiscard]] inline auto group_mask() const noexcept {
    return std::array{true, answers, timing};
  }

  [[nodiscard]] inline auto content_mask() const noexcept {
    return std::array{true, part1 and answers, part2 and answers, timing, part1 and timing, part2 and timing, timing};
  }

  [[nodiscard]] inline auto summary_mask() const noexcept {
    return std::array{true, timing, timing, timing, timing};
  }

  [[nodiscard]] bool validate() const noexcept;
};
