#pragma once

#include <cstdio>
#include <type_traits>
#include <optional>
#include <span>

#include "Types.hpp"

template <
    typename ParseResult,
    typename Part1Result,
    typename Part2Result = Part1Result
>
struct Day {

    using parse_result_t = ParseResult;
    using part1_result_t = Part1Result;
    using part2_result_t = Part2Result;

    parse_result_t parse(std::span<char const> f) const noexcept;

    template <bool Part2>
    std::conditional_t<Part2, part2_result_t, part1_result_t>
    solve(const_bool<Part2>, parse_result_t const& parsed, [[maybe_unused]] std::optional<part1_result_t> part1_answer = std::nullopt) const noexcept;
};

//! Macro for generating function signature for parse
#define PARSE_IMPL(DAY, ParamBuffer) \
/* Class template specialization */ \
template <> \
typename DAY::parse_result_t DAY::parse(std::span<char const> ParamBuffer) const noexcept


//! Macro for generating function signature for a part1 solution
#define PART1_IMPL(DAY, ParamParseResult) \
/* Class template specialization */ \
template <> \
/* Function template specialization */ \
template <> \
typename DAY::part1_result_t \
DAY::solve(false_type, typename DAY::parse_result_t const& ParamParseResult, std::optional<typename DAY::part1_result_t>) const noexcept


//! Macro for generating function signature for a part2 solution
#define PART2_IMPL(DAY, ParamParseResult, ParamPart1Answer) \
/* Class template specialization */ \
template <> \
/* Function template specialization */ \
template <> \
typename DAY::part2_result_t \
DAY::solve(true_type, typename DAY::parse_result_t const& ParamParseResult, [[maybe_unused]] std::optional<typename DAY::part1_result_t> ParamPart1Answer) const noexcept

//! Macro for generating function signature for generic solve.
/*! \note Must later call INSTANTIATE() in the same TU for codegen
*/
#define SOLVE_IMPL(DAY, TParamPart2, ParamParseResult, ParamPart1Answer) \
/* Class template specialization */ \
template <> \
/* Function template */ \
template <bool TParamPart2> \
std::conditional_t<TParamPart2, typename DAY::part2_result_t, typename DAY::part1_result_t> \
DAY::solve(const_bool<TParamPart2>, typename DAY::parse_result_t const& ParamParseResult, [[maybe_unused]] std::optional<typename DAY::part1_result_t> ParamPart1Answer) const noexcept

//! Explicitly instatiate templated SOLVE_IMPL implementation
#define INSTANTIATE(DAY) \
template typename DAY::part1_result_t DAY::solve<false>(const_bool<false>, parse_result_t const&, std::optional<part1_result_t>) const noexcept; \
template typename DAY::part2_result_t DAY::solve<true>(const_bool<true>, parse_result_t const&, std::optional<part1_result_t>) const noexcept
