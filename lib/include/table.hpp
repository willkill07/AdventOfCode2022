#pragma once

#include <array>
#include <vector>

#include "options.hpp"
#include "timing.hpp"

constexpr usize const report_size = 7;

using report_line = std::array<std::string, report_size>;

using report_data = std::vector<report_line>;

void
print(run_options const &options, report_data const &entries, timing_data const &summary);
