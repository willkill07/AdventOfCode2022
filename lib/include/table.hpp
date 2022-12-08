#pragma once

#include <array>
#include <vector>

#include "options.hpp"
#include "timing.hpp"

constexpr usize const report_size = 7;

enum class index : u32 {
  day = 0u,
  part1_answer = 1u,
  part2_answer = 2u,
  parse_time = 3u,
  part1_time = 4u,
  part2_time = 5u,
  total_time = 6u
};

using report_line = std::array<std::string, report_size>;

using report_data = std::vector<report_line>;
using report_timing = std::vector<timing_data>;

void
print(run_options const &options, report_data const &entries, timing_data const &summary);
