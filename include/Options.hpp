#pragma once

#include <concepts>
#include <optional>
#include <string>

#include <fmt/core.h>

#include "Types.hpp"

struct run_options {
  u32 precision{2};
  bool timing{false};
  bool part2{true};
  std::optional<u32> single{std::nullopt};

  inline std::string format(std::integral auto value) const noexcept {
    return fmt::format("{0}", value);
  }

  inline std::string format(std::floating_point auto value) const noexcept {
    return fmt::format("{0:.{1}f}", value, precision);
  }

  inline std::string const &format(std::string const &value) const noexcept {
    return value;
  }

  inline std::string &&format(std::string &&value) const noexcept {
    return value;
  }
};