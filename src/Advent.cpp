#include <cstdlib>
#include <tuple>
#include <unistd.h>
#include <utility>
#include <vector>

#include <fmt/core.h>

#include "Advent.hpp"
#include "FileBackedBuffer.hpp"
#include "FixedString.hpp"
#include "Options.hpp"
#include "TabularPrint.hpp"
#include "Timing.hpp"
#include "Types.hpp"

template <sz DayIdx>
timing_data run_one(report_data &data, run_options const &options) {
  using CurrentDay = std::tuple_element_t<DayIdx, all_days>;

  file_backed_buffer buffer{
      fmt::format("input/day{:02}.txt", CurrentDay::number)};
  if (not buffer) {
    return {};
  }

  CurrentDay day;

  time_point t0 = clock_type::now();
  auto const parsed = day.parse(buffer.get_span());
  time_point t1 = clock_type::now();
  auto const part1_answer = day.solve(false_v, parsed);
  time_point t2 = clock_type::now();
  auto const part2_answer = [&] {
    if (options.part2) {
      return day.solve(true_v, parsed, part1_answer);
    } else {
      return typename CurrentDay::part2_result_t{};
    }
  }();
  time_point t3 = clock_type::now();

  timing_data const curr{.parsing = time_in_us(t0, t1),
                         .part1 = time_in_us(t1, t2),
                         .part2 = time_in_us(t2, t3)};

  data[DayIdx] = report_line{fmt::format("Day {:02}", CurrentDay::number),
                             options.format(part1_answer),
                             options.format(part2_answer),
                             options.format(curr.parsing),
                             options.format(curr.part1),
                             options.format(curr.part2),
                             options.format(curr.total())};

  return curr;
}

std::tuple<timing_data, report_data> run(run_options const &options) noexcept {
  report_data entries;
  timing_data summary;
  auto const do_run = [&]<sz CurrDay>(const_sz<CurrDay>) {
    summary += run_one<CurrDay>(entries, options);
  };
  [&]<sz... Days>(std::index_sequence<Days...>) {
    (do_run(const_sz<Days>{}), ...);
  }
  (std::make_index_sequence<implemented_days>{});
  return std::tuple{summary, entries};
}

int main(int argc, char **argv) {
  run_options options;
  bool help{false}, error{false};

  for (int c; (c = getopt(argc, argv, "p:d:th")) != -1;) {
    switch (c) {
    case 'p':
      options.precision = static_cast<u32>(atoi(optarg));
      break;
    case 'd':
      options.part2 = (atoi(optarg) == 2);
      break;
    case 't':
      options.timing = true;
      break;
    case 'h':
      help = true;
      break;
    case '?':
      help = true;
      if (optopt == 'p' || optopt == 'd') {
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      } else if (isprint(optopt)) {
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      } else {
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      }
      error = true;
      break;
    default:
      help = true;
      error = true;
    }
  }

  if (help) {
    fmt::print("Advent of Code 2022 (in Modern C++)\n");
    fmt::print("Created by William Killian (willkill07)\n\n");
    fmt::print("Usage: {} [-d [1|2]] [-t] [-p P] [-h]\n\n", argv[0]);
    fmt::print("    -d [1|2]     parts to run (defaults to 2)\n");
    fmt::print("    -t           show timing (disabled by default)\n");
    fmt::print("    -p P         precision of timing output (default 2)\n");
    fmt::print("    -h           show help\n");
    if (error) {
      return EXIT_FAILURE;
    } else {
      return EXIT_SUCCESS;
    }
  }

  auto [summary, entries] = run(options);

  print_table(options, entries, summary);
}
