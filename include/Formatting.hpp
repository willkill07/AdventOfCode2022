#pragma once

#include <fmt/color.h>
#include <fmt/core.h>

template <typename T>
using FormatFn = fmt::detail::styled_arg<std::remove_cvref_t<T>> (*)(T const &) noexcept;

template <fmt::emphasis... Emphs>
fmt::detail::styled_arg<std::string>
print_plain(std::string const &v) noexcept {
  if (isatty(STDOUT_FILENO)) {
    return fmt::styled(v, (fmt::text_style{} | ... | fmt::text_style{Emphs}));
  } else {
    return fmt::styled(v, fmt::text_style{});
  }
}

template <fmt::color Color, fmt::emphasis... Emphs>
fmt::detail::styled_arg<std::string>
print(std::string const &v) noexcept {
  if (isatty(STDOUT_FILENO)) {
    return fmt::styled(v, (fmt::fg(Color) | ... | fmt::text_style{Emphs}));
  } else {
    return fmt::styled(v, fmt::text_style{});
  }
}

namespace colors {

constexpr auto header_plain = &print_plain<fmt::emphasis::bold, fmt::emphasis::italic>;
constexpr auto plain = &print_plain<>;

constexpr auto red = &print<fmt::color::red>;
constexpr auto yellow = &print<fmt::color::yellow>;
constexpr auto green = &print<fmt::color::green>;
constexpr auto cyan = &print<fmt::color::cyan>;

constexpr auto bold_red = &print<fmt::color::red, fmt::emphasis::bold>;
constexpr auto bold_yellow = &print<fmt::color::yellow, fmt::emphasis::bold>;
constexpr auto bold_green = &print<fmt::color::green, fmt::emphasis::bold>;
constexpr auto bold_cyan = &print<fmt::color::cyan, fmt::emphasis::bold>;

constexpr auto faint_red = &print<fmt::color::red, fmt::emphasis::faint>;
constexpr auto faint_green = &print<fmt::color::green, fmt::emphasis::faint>;
constexpr auto faint_yellow = &print<fmt::color::yellow, fmt::emphasis::faint>;
constexpr auto faint_cyan = &print<fmt::color::cyan, fmt::emphasis::faint>;

} // namespace colors
