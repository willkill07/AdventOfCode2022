#pragma once

#include <cstdio>

#include "chart/calculator.hpp"
#include "fixed_string.hpp"

namespace chart {

enum class orientation { horizontal, vertical };

namespace detail {

template <fixed_string Whole, fixed_string Parts>
struct printable {
  static constexpr std::string_view const whole = fixed_u8string<Whole>{}[0];
  static constexpr fixed_u8string<Parts> const parts{};
  static constexpr std::size_t const width = parts.size();
};

using horizontal = printable<"█", " ▏▎▍▌▋▊▉█">;
using vertical = printable<"█", " ▁▂▃▄▅▆▇█">;

} // namespace detail

template <orientation Layout>
using characters_for = std::conditional_t<Layout == orientation::horizontal, detail::horizontal, detail::vertical>;

template <unsigned Parts>
std::string
row(length_result<Parts> res) {
  using Chars = characters_for<orientation::horizontal>;
  std::string result;
  for (unsigned i{0}; i < res.whole_count; ++i) {
    result.append(Chars::whole);
  }
  if (res.part > 0) {
    result.append(Chars::parts[res.part]);
  }
  result.append(res.spacing, ' ');
  return result;
}

} // namespace chart
