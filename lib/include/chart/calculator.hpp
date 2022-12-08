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
    unsigned const segments = width * Parts;
    unsigned const percentage = static_cast<unsigned>(segments * (static_cast<double>(value) / static_cast<double>(max_value)));
    unsigned const whole = percentage / Parts;
    unsigned const partial = 1 + percentage % Parts;
    if (whole == width) {
      return {whole, 0, 0};
    }
    if (whole == 0 and partial == 0) {
      return {0, 1, width - 1};
    }
    width -= whole + 1;
    return {whole, partial, width};
  }
};

} // namespace chart
