#pragma once

#include <cstdio>
#include <type_traits>
#include <optional>

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

    parse_result_t parse(FILE* f) const noexcept;

    template <bool Part2>
    std::conditional_t<Part2, part2_result_t, part1_result_t>
    solve(const_bool<Part2>, parse_result_t const& parsed, [[maybe_unused]] std::optional<part1_result_t> part1_answer = std::nullopt) const noexcept;
};

#define INSTANTIATE(DAY) \
template typename DAY::part1_result_t DAY::solve<false>(const_bool<false>, parse_result_t const&, std::optional<part1_result_t>) const noexcept; \
template typename DAY::part2_result_t DAY::solve<true>(const_bool<true>, parse_result_t const&, std::optional<part1_result_t>) const noexcept
