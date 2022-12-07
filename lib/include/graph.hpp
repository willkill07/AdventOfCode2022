#pragma once

#include "options.hpp"
#include "table.hpp"
#include "timing.hpp"

void
graph_output(run_options const &options,
             std::vector<timing_data> const &timing,
             std::vector<report_line> const &entries) noexcept;
