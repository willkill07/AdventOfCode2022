#pragma once

#include <tuple>
#include <type_traits>

#include "Day01.hpp"

using all_days = std::tuple<Day01>;



constexpr const sz implemented_days = std::tuple_size_v<all_days>;
