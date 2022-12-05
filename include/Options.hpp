#pragma once

#include <concepts>
#include <optional>
#include <string>

#include <fmt/core.h>

#include "Types.hpp"

struct run_options {
  u32 precision{2};
  bool colorize{true};
  bool timing{true};
  bool part2{true};
  bool part1{true};
  std::optional<u32> single{std::nullopt};
  std::optional<u32> benchmark{std::nullopt};

  inline std::string format(std::integral auto value) const noexcept {
    return fmt::format("{0}", value);
  }

  inline std::string format(std::floating_point auto value) const noexcept {
    return fmt::format("{0:.{1}f}", value, precision);
  }

  inline std::string const &format(std::string const &value) const noexcept {
    return value;
  }

  inline std::string format(std::string &&value) const noexcept {
    return std::string{value};
  }

  inline auto group_mask() const noexcept {
    return std::array{true, true, timing};
  }

  inline auto content_mask() const noexcept {
    return std::array{true, part1, part2, timing, part1 && timing, part2 && timing, timing};
  }

  inline auto summary_mask() const noexcept {
    return std::array{true, timing, timing, timing, timing};
  }

};