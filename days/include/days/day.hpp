#pragma once

#include <cstdio>
#include <optional>
#include <string_view>
#include <type_traits>

#ifndef DOCTEST_CONFIG_DISABLE
#include <doctest/doctest.h>
#endif

#include "types.hpp"

using std::string_view_literals::operator""sv;

template <u32 Number, typename ParseResult, typename Part1Result, typename Part2Result = Part1Result>
struct Day {
  constexpr static u32 const number{Number};

  using parse_result_t = ParseResult;
  using part1_result_t = Part1Result;
  using part2_result_t = Part2Result;

  [[nodiscard]] parse_result_t parse_input(std::string_view) const noexcept;

  [[nodiscard]] inline part1_result_t part1(parse_result_t const &parsed) const noexcept {
    return solve<false>(parsed, {});
  }

  [[nodiscard]] inline part2_result_t part2(parse_result_t const &parsed,
                                            std::optional<part1_result_t> part1_answer = std::nullopt) const noexcept {
    return solve<true>(parsed, part1_answer);
  }

private:
  template <bool Part2>
  [[nodiscard]] std::conditional_t<Part2, part2_result_t, part1_result_t>
  solve(parse_result_t const &parsed, [[maybe_unused]] std::optional<part1_result_t> part1_answer) const noexcept;
};

//! Macro for generating function signature for parse
#define PARSE_IMPL(DAY, ParamBuffer)                                                                                   \
  /* Class template specialization */                                                                                  \
  template <>                                                                                                          \
  [[nodiscard]] typename DAY::parse_result_t DAY::parse_input(std::string_view ParamBuffer) const noexcept

//! Macro for generating function signature for a part1 solution
#define PART1_IMPL(DAY, ParamParseResult)                                                                              \
  /* Class template specialization */                                                                                  \
  template <> /* Function template specialization */                                                                   \
  template <>                                                                                                          \
  [[nodiscard]] typename DAY::part1_result_t DAY::solve<false>(typename DAY::parse_result_t const &ParamParseResult,   \
                                                               std::optional<typename DAY::part1_result_t>)            \
      const noexcept

//! Macro for generating function signature for a part2 solution
#define PART2_IMPL(DAY, ParamParseResult, ParamPart1Answer)                                                            \
  /* Class template specialization */                                                                                  \
  template <> /* Function template specialization */                                                                   \
  template <>                                                                                                          \
  [[nodiscard]] typename DAY::part2_result_t DAY::solve<true>(                                                         \
      typename DAY::parse_result_t const &ParamParseResult,                                                            \
      [[maybe_unused]] std::optional<typename DAY::part1_result_t> ParamPart1Answer) const noexcept

//! Macro for generating function signature for generic solve.
/*! \note Must later call INSTANTIATE() in the same TU for codegen
 */
#define SOLVE_IMPL(DAY, TParamPart2, ParamParseResult, ParamPart1Answer)                                               \
  /* Class template specialization */                                                                                  \
  template <> /* Function template */                                                                                  \
  template <bool TParamPart2>                                                                                          \
  [[nodiscard]] std::conditional_t<TParamPart2, typename DAY::part2_result_t, typename DAY::part1_result_t>            \
  DAY::solve(typename DAY::parse_result_t const &ParamParseResult,                                                     \
             [[maybe_unused]] std::optional<typename DAY::part1_result_t> ParamPart1Answer) const noexcept

//! Explicitly instatiate templated SOLVE_IMPL implementation
#define INSTANTIATE(DAY)                                                                                               \
  template typename DAY::part1_result_t DAY::solve<false>(parse_result_t const &, std::optional<part1_result_t>)       \
      const noexcept;                                                                                                  \
  template typename DAY::part2_result_t DAY::solve<true>(parse_result_t const &, std::optional<part1_result_t>)        \
      const noexcept

#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#ifdef DOCTEST_CONFIG_DISABLE
#define INSTANTIATE_TEST(Day, Input, Part1Answer, Part2Answer)
#else
#define INSTANTIATE_TEST(Day, Input, Part1Answer, Part2Answer)                                                         \
  TEST_CASE(#Day) {                                                                                                    \
    Day day;                                                                                                           \
    auto const view = day.parse_input(Input);                                                                          \
    auto const part1_actual = day.part1(view);                                                                         \
    auto const part2_actual = day.part2(view, part1_actual);                                                           \
    SUBCASE("Part 1") {                                                                                                \
      CHECK_EQ(part1_actual, Part1Answer);                                                                             \
    }                                                                                                                  \
    SUBCASE("Part 2") {                                                                                                \
      CHECK_EQ(part2_actual, Part2Answer);                                                                             \
    }                                                                                                                  \
  }
#endif
