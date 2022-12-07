#pragma once

#include <concepts>
#include <numeric>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "fixed_string.hpp"
#include "meta/utils.hpp"

template <typename T>
T parse(std::string_view) noexcept;

template <>
[[gnu::always_inline, gnu::flatten, nodiscard]] inline char
parse<char>(std::string_view s) noexcept {
  if (s.empty()) {
    return {};
  } else {
    return s[0];
  }
}

template <>
[[gnu::always_inline, gnu::flatten, nodiscard]] inline std::string_view
parse<std::string_view>(std::string_view s) noexcept {
  return s;
}

template <std::unsigned_integral T>
[[gnu::always_inline, gnu::flatten, nodiscard]] inline T
parse(std::string_view s) noexcept {
  if (s.empty()) {
    return {};
  } else {
    return std::accumulate(std::begin(s) + 1, std::end(s), static_cast<T>(s.front() - '0'), [](T acc, char c) {
      return T{10} * acc + T(c - '0');
    });
  }
}

template <std::signed_integral T>
[[gnu::always_inline, gnu::flatten, nodiscard]] inline T
parse(std::string_view s) noexcept {
  if (s.empty()) {
    return {};
  } else {
    bool const neg{s.front() == '-'};
    T const value{static_cast<T>(parse<std::make_unsigned_t<T>>(s.substr(static_cast<std::size_t>(neg))))};
    return neg ? -value : value;
  }
}

template <std::floating_point T>
[[gnu::always_inline, gnu::flatten, nodiscard]] inline T
parse(std::string_view s) noexcept {
  bool neg{s.front() == '-'};
  if (neg) {
    s = s.substr(1);
  }
  double whole{0};
  while (not s.empty() and s[0] != '.') {
    whole = whole * 10 + (s[0] - '0');
    s = s.substr(1);
  }
  if (s.empty()) {
    return static_cast<T>(whole);
  }
  // skip dot
  s = s.substr(1);
  double frac{0};
  double div{0};
  while (not s.empty()) {
    frac = frac * 10 + (s[0] - '0');
    div *= 10;
    s = s.substr(1);
  }
  return whole + (frac / div);
}

template <fixed_string FormatStr, typename... Ts>
[[gnu::always_inline, gnu::flatten, nodiscard]] inline std::size_t
parse(std::string_view view, Ts &...vals) noexcept {
  using Types = std::tuple<Ts...>;
  constexpr std::size_t const Elems = sizeof...(Ts);
  return fold<FormatStr.size()>(
      std::size_t{0},
      []<std::size_t CurrIndex>(std::size_t &off,
                                std::integral_constant<std::size_t, CurrIndex>,
                                std::string_view const &v,
                                auto &&vs) {
        constexpr char const CurrChar{FormatStr[CurrIndex]};
        if constexpr (is_placeholder_char(CurrChar, Elems)) {
          static_assert(FormatStr.check(CurrIndex + 1), "Must have trailing character after placeholder");
          constexpr auto const NextChar = FormatStr[CurrIndex + 1];
          static_assert(not is_placeholder_char(NextChar, Elems), "Two placeholders must not be adjacent");
          using T = std::tuple_element_t<CurrChar, Types>;
          // calculate length of string_view matching criteria
          std::size_t const len = v.substr(off).find_first_of(NextChar);
          // update reference with parsed value
          std::get<CurrChar>(vs) = parse<T>(v.substr(off, len));
          // consume the char after the placeholder too
          off += len;
        } else if constexpr (CurrIndex > 0) {
          constexpr auto const PrevChar = FormatStr[CurrIndex - 1];
          if constexpr (is_placeholder_char(PrevChar, Elems)) {
            // do not advance if we are preceded by a placeholder
            --off;
          }
        }
        ++off;
      },
      view,
      std::tie(vals...));
}
