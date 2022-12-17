#pragma once
#include <tuple>

#include "types.hpp"

#include "days/day01.hpp"
#include "days/day02.hpp"
#include "days/day03.hpp"
#include "days/day04.hpp"
#include "days/day05.hpp"
#include "days/day06.hpp"
#include "days/day07.hpp"
#include "days/day08.hpp"
#include "days/day09.hpp"
#include "days/day10.hpp"
#include "days/day11.hpp"
#include "days/day12.hpp"
#include "days/day13.hpp"
#include "days/day14.hpp"
#include "days/day15.hpp"
#include "days/day16.hpp"

using all_days =
    std::tuple<Day01, Day02, Day03, Day04, Day05, Day06, Day07, Day08, Day09, Day10, Day11, Day12, Day13, Day14, Day15, Day16>;

constexpr inline static usize const implemented_days = std::tuple_size_v<all_days>;
