#pragma once

#include <concepts>
#include <numeric>
#include <string_view>
#include <tuple>
#include <utility>

#include "FixedString.hpp"

template <typename T>
T read(std::string_view) noexcept;

template <>
[[gnu::always_inline, gnu::flatten]] inline char
read<char>(std::string_view s) noexcept {
  if (s.empty())
    return {};
  return s[0];
}

template <>
[[gnu::always_inline, gnu::flatten]] inline std::string_view
read<std::string_view>(std::string_view s) noexcept {
  return s;
}

template <std::unsigned_integral T>
[[gnu::always_inline, gnu::flatten]] inline T
read(std::string_view s) noexcept
  requires std::same_as<T, u32> or std::same_as<T, u64>
{
  if (s.empty())
    return {};
  return std::accumulate(std::begin(s) + 1, std::end(s), static_cast<T>(s.front() - '0'), [](T acc, char c) {
    return T{10} * acc + T(c - '0');
  });
}

template <std::signed_integral T>
[[gnu::always_inline, gnu::flatten]] inline T
read(std::string_view s) noexcept
  requires std::same_as<T, i32> or std::same_as<T, i64>
{
  if (s.empty())
    return {};
  bool const neg{s.front() == '-'};
  T const value{static_cast<T>(read<std::make_unsigned_t<T>>(s.substr(static_cast<sz>(neg))))};
  return neg ? -value : value;
}

template <fixed_string FormatStr, typename... Ts>
[[gnu::always_inline, gnu::flatten]] inline sz
read(std::string_view view, Ts &...vals) noexcept {
  using Types = std::tuple<Ts...>;
  constexpr sz const Elems = sizeof...(Ts);
  auto values = std::tie(vals...);
  sz offset{0};
  iterate<FormatStr>(
      []<sz CurrIndex, char CurrChar> [[gnu::flatten, gnu::always_inline]] (auto const &v, auto &off, auto &vals) {
        constexpr auto implies = [](bool a, bool b) constexpr noexcept {
          return !a or b;
        };
        constexpr auto in_bounds = []<sz N>(sz index, fixed_string<N> str) constexpr noexcept {
          return index < str.size;
        };
        constexpr auto is_placeholder = [](char c) constexpr noexcept {
          return static_cast<sz>(c) < Elems;
        };
        if constexpr (is_placeholder(CurrChar)) {
          static_assert(in_bounds(CurrIndex + 1, FormatStr), "Must have trailing character after placeholder");
          static_assert(implies(in_bounds(CurrIndex + 1, FormatStr), not is_placeholder(FormatStr(CurrIndex + 1))),
                        "Two placeholders must not be adjacent");
          sz const len = v.find_first_of(FormatStr(CurrIndex + 1), off) - off;
          using T = std::tuple_element_t<CurrChar, Types>;
          std::get<CurrChar>(vals) = read<T>(v.substr(off, len));
          // consume the char after the placeholder too
          off += len + 1;
        } else if constexpr (CurrIndex > 0 and not is_placeholder(FormatStr(CurrIndex - 1))) {
          // only advance if we are first or if we are NOT preceded by a placeholder
          ++off;
        }
      },
      view,
      offset,
      values);
  return offset;
}
