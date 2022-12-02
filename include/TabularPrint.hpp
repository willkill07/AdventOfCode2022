#pragma once

#include <vector>

#include "Options.hpp"
#include "Timing.hpp"

void print_table(run_options const &options, report_data const &entries,
                 timing_data const &summary);
