#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

#include <fmt/core.h>

namespace chart {

template <unsigned Parts>
struct length_result {
  unsigned whole_count;
  unsigned part;
  unsigned spacing;
};

template <typename T, unsigned Parts>
class calculator {

  T max_value{std::numeric_limits<T>::min()};

public:
  inline void add_sample(T value) noexcept {
    max_value = std::max(max_value, value);
  }

  length_result<Parts> length_for(T value, unsigned width) {
    value = std::clamp(value, T{0}, max_value);
    double const scaled{static_cast<double>(width) * (static_cast<double>(value) / max_value)};
    unsigned const whole{static_cast<unsigned>(std::max({0.0, scaled - 1}))};
    unsigned const remainder{static_cast<unsigned>((scaled - whole) * Parts)};
    unsigned const spacing{static_cast<unsigned>(width - (whole + 1))};
    return {whole, remainder % Parts, spacing};
  }
};

} // namespace chart
