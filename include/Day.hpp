#pragma once

#include <cstdio>
#include <optional>
#include <string_view>
#include <type_traits>

#include <doctest/doctest.h>

using std::string_view_literals::operator""sv;

#include "Types.hpp"

template <u32 Number, typename ParseResult, typename Part1Result, typename Part2Result = Part1Result>
struct Day {
  constexpr static u32 const number{Number};

  using parse_result_t = ParseResult;
  using part1_result_t = Part1Result;
  using part2_result_t = Part2Result;

  parse_result_t parse(std::string_view) const noexcept;

  template <bool Part2>
  std::conditional_t<Part2, part2_result_t, part1_result_t>
  solve(parse_result_t const &parsed,
        [[maybe_unused]] std::optional<part1_result_t> part1_answer = std::nullopt) const noexcept;
};

//! Macro for generating function signature for parse
#define PARSE_IMPL(DAY, ParamBuffer)                                                                                   \
  /* Class template specialization */                                                                                  \
  template <>                                                                                                          \
  typename DAY::parse_result_t DAY::parse(std::string_view ParamBuffer) const noexcept

//! Macro for generating function signature for a part1 solution
#define PART1_IMPL(DAY, ParamParseResult)                                                                              \
  /* Class template specialization */                                                                                  \
  template <> /* Function template specialization */                                                                   \
  template <>                                                                                                          \
  typename DAY::part1_result_t DAY::solve<false>(typename DAY::parse_result_t const &ParamParseResult,                 \
                                                 std::optional<typename DAY::part1_result_t>) const noexcept

//! Macro for generating function signature for a part2 solution
#define PART2_IMPL(DAY, ParamParseResult, ParamPart1Answer)                                                            \
  /* Class template specialization */                                                                                  \
  template <> /* Function template specialization */                                                                   \
  template <>                                                                                                          \
  typename DAY::part2_result_t DAY::solve<true>(                                                                       \
      typename DAY::parse_result_t const &ParamParseResult,                                                            \
      [[maybe_unused]] std::optional<typename DAY::part1_result_t> ParamPart1Answer) const noexcept

//! Macro for generating function signature for generic solve.
/*! \note Must later call INSTANTIATE() in the same TU for codegen
 */
#define SOLVE_IMPL(DAY, TParamPart2, ParamParseResult, ParamPart1Answer)                                               \
  /* Class template specialization */                                                                                  \
  template <> /* Function template */                                                                                  \
  template <bool TParamPart2>                                                                                          \
  std::conditional_t<TParamPart2, typename DAY::part2_result_t, typename DAY::part1_result_t> DAY::solve(              \
      typename DAY::parse_result_t const &ParamParseResult,                                                            \
      [[maybe_unused]] std::optional<typename DAY::part1_result_t> ParamPart1Answer) const noexcept

//! Explicitly instatiate templated SOLVE_IMPL implementation
#define INSTANTIATE(DAY)                                                                                               \
  template typename DAY::part1_result_t DAY::solve<false>(parse_result_t const &, std::optional<part1_result_t>)       \
      const noexcept;                                                                                                  \
  template typename DAY::part2_result_t DAY::solve<true>(parse_result_t const &, std::optional<part1_result_t>)        \
      const noexcept
