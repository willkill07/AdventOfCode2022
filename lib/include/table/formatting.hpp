#pragma once

#include <unistd.h>

#include <fmt/color.h>
#include <fmt/core.h>

#include "fixed_string.hpp"
#include "types.hpp"

namespace table {

template <typename T>
using format_function_t = fmt::detail::styled_arg<std::remove_cvref_t<T>> (*)(T) noexcept;

// singleton static flag for fast (done-once TTY detection)
inline bool const is_a_tty{static_cast<bool>(isatty(STDOUT_FILENO))};

template <fmt::emphasis... Emphs>
fmt::detail::styled_arg<std::string>
print_plain(std::string v) noexcept {
  fmt::text_style style{};
  if (is_a_tty) {
    style = (style | ... | fmt::text_style{Emphs});
  }
  return fmt::styled(v, style);
}

template <fmt::detail::color_type Color, fmt::emphasis... Emphs>
fmt::detail::styled_arg<std::string>
print(std::string v) noexcept {
  fmt::text_style style{};
  if (is_a_tty) {
    style = (fmt::fg(Color) | ... | fmt::text_style{Emphs});
  }
  return fmt::styled(v, style);
}

namespace colors {

inline constexpr auto header_plain = &print_plain<fmt::emphasis::bold, fmt::emphasis::italic>;
inline constexpr auto plain = &print_plain<>;

inline constexpr auto red = &print<fmt::terminal_color::red>;
inline constexpr auto yellow = &print<fmt::terminal_color::yellow>;
inline constexpr auto green = &print<fmt::terminal_color::green>;
inline constexpr auto cyan = &print<fmt::terminal_color::cyan>;

inline constexpr auto bold_red = &print<fmt::terminal_color::red, fmt::emphasis::bold>;
inline constexpr auto bold_yellow = &print<fmt::terminal_color::yellow, fmt::emphasis::bold>;
inline constexpr auto bold_green = &print<fmt::terminal_color::green, fmt::emphasis::bold>;
inline constexpr auto bold_cyan = &print<fmt::terminal_color::cyan, fmt::emphasis::bold>;

inline constexpr auto faint_red = &print<fmt::terminal_color::red, fmt::emphasis::faint>;
inline constexpr auto faint_green = &print<fmt::terminal_color::green, fmt::emphasis::faint>;
inline constexpr auto faint_yellow = &print<fmt::terminal_color::yellow, fmt::emphasis::faint>;
inline constexpr auto faint_cyan = &print<fmt::terminal_color::cyan, fmt::emphasis::faint>;

} // namespace colors

template <usize Width>
inline std::array<format_function_t<std::string>, Width>
maybe_plain(bool color_condition, std::array<format_function_t<std::string>, Width> arr) noexcept {
  if (not color_condition) {
    arr.fill(colors::plain);
  }
  return arr;
}

template <fixed_string S, usize Width>
void
print_edge_row(std::array<usize, Width> const &widths) {
  static constexpr fixed_u8string<S> String{};
  static_assert(String.size() == Width * 2 + 1, "table format string mismatch for width");

  usize skipped{0};
#if defined(__clang__)
#pragma unroll
#endif
  for (usize I{0}; I < Width; ++I) {
    if (widths[I] > 0) {
      auto const Left = String[2 * (I - skipped)];
      auto const Repeat = String[2 * I + 1];
      auto const ColWidth = widths[I] + 2;
      fmt::vprint(fmt::format("{}{{:{}^{}}}", Left, Repeat, ColWidth), fmt::make_format_args(""));
      skipped = 0;
    } else {
      ++skipped;
    }
  }
  fmt::print("{}\n", String[Width * 2]);
}

template <fixed_string S,
          typename T,
          usize Data,
          usize Width,
          usize Style,
          fixed_u8string<S> String = fixed_u8string<S>{}>
void
print_data_row(std::array<T, Data> const &data,
               std::array<usize, Width> const &widths,
               std::array<format_function_t<std::string>, Style> const &stylizers) noexcept {
  constexpr usize const elements = (String.size() - 1) / 2;
  static_assert((String.size() & 1) == 1, "table format string must have odd length");
  static_assert(String.size() > 2, "table format string must be at least 3");
  static_assert(elements == Width, "table format string mismatch for width");
  static_assert(elements == Data, "table format string mismatch for data");
  static_assert(elements == Style, "table format string mismatch for stylizers");

  for (usize I{0}; I < Width; ++I) {
    auto const Align = String[2 * I + 1];
    auto const Border = String[2 * I];
    auto const ColWidth = widths[I];
    if (ColWidth > 0) {
      fmt::vprint(fmt::format("{{}} {{: {}{}}} ", Align, ColWidth),
                  fmt::make_format_args(Border, stylizers[I](data[I])));
    }
  }
  fmt::print("{}\n", String[Width * 2]);
}

} // namespace table
