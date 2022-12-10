#pragma once

#include <array>
#include <concepts>
#include <optional>
#include <string>
#include <string_view>

#include <fmt/core.h>

#include "types.hpp"

struct run_options {
  constexpr inline static u32 default_bar_width{8};
  constexpr inline static u32 default_precision{2};
  constexpr inline static u32 default_graph_width{50};

  std::optional<u32> precision{std::nullopt};
  std::optional<u32> graph_width{std::nullopt};
  std::optional<u32> single{std::nullopt};
  std::optional<u32> benchmark{std::nullopt};
  
  bool timing{true};
  bool part2{true};
  bool part1{true};
  bool answers{true};
  bool mask{false};
  bool colorize{true};
  bool graphs{false};
  bool visual{false};


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
    if (mask) {
      return std::string(fmt::formatted_size("{}", std::forward<T>(value)), 'X');
    } else {
      return format(std::forward<T>(value));
    }
  }

  [[nodiscard]] inline auto group_mask() const noexcept {
    return std::array{true, answers, timing};
  }

  [[nodiscard]] inline auto content_mask() const noexcept {
    return std::array{true, part1 and answers, part2 and answers, timing, part1 and timing, part2 and timing, timing};
  }

  [[nodiscard]] inline auto update_mask() const noexcept {
    bool const show{timing and not visual};
    return std::array{true, part1 and answers, part2 and answers, show, part1 and show, part2 and show, show};
  }

  [[nodiscard]] inline auto summary_mask() const noexcept {
    return std::array{true, timing, timing, timing, timing};
  }

  [[nodiscard]] bool validate() const noexcept;
};
