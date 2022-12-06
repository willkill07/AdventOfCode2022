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
  T const value{static_cast<T>(read<std::make_unsigned_t<T>>(s.substr(static_cast<usize>(neg))))};
  return neg ? -value : value;
}

template <fixed_string FormatStr, typename... Ts>
[[gnu::always_inline, gnu::flatten]] inline usize
read(std::string_view view, Ts &...vals) noexcept {
  using Types = std::tuple<Ts...>;
  constexpr usize const Elems = sizeof...(Ts);
  auto values = std::tie(vals...);
  usize offset{0};
  iterate<FormatStr>(
      []<usize CurrIndex, char CurrChar> [[gnu::flatten, gnu::always_inline]] (auto const &v, auto &off, auto &vs) {
        constexpr auto implies = [](bool a, bool b) constexpr noexcept {
          return !a or b;
        };
        constexpr auto in_bounds = []<usize N>(usize index, fixed_string<N> str) constexpr noexcept {
          return index < str.size;
        };
        constexpr auto is_placeholder = [](char c) constexpr noexcept {
          return static_cast<usize>(c) < Elems;
        };
        if constexpr (is_placeholder(CurrChar)) {
          static_assert(in_bounds(CurrIndex + 1, FormatStr), "Must have trailing character after placeholder");
          static_assert(implies(in_bounds(CurrIndex + 1, FormatStr), not is_placeholder(FormatStr(CurrIndex + 1))),
                        "Two placeholders must not be adjacent");
          using T = std::tuple_element_t<CurrChar, Types>;
          // calculate length of string_view matching criteria
          usize const len = v.find_first_of(FormatStr(CurrIndex + 1), off) - off;
          // update reference with parsed value
          std::get<CurrChar>(vs) = read<T>(v.substr(off, len));
          // consume the char after the placeholder too
          off += len;
        } else if constexpr (CurrIndex > 0 and is_placeholder(FormatStr(CurrIndex - 1))) {
          // do not advance if we are preceded by a placeholder
          --off;
        }
        ++off;
      },
      view,
      offset,
      values);
  return offset;
}
