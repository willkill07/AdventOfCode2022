#include <cstdlib>
#include <tuple>
#include <utility>
#include <vector>

#include <unistd.h>

#include <fmt/core.h>

#include "Advent.hpp"
#include "FileBackedBuffer.hpp"
#include "FixedString.hpp"
#include "MetaUtils.hpp"
#include "Options.hpp"
#include "TabularPrint.hpp"
#include "Timing.hpp"
#include "Types.hpp"

template <sz DayIdx>
timing_data
run_one(report_data &data, run_options const &options) {
  using CurrentDay = std::tuple_element_t<DayIdx, all_days>;

  if (options.single.has_value() && options.single.value() != DayIdx) {
    return {};
  }

  file_backed_buffer buffer{fmt::format("input/day{:02}.txt", CurrentDay::number)};
  if (not buffer) {
    return {};
  }

  CurrentDay day;
  time_point t0 = clock_type::now();
  auto const parsed = [&] {
    for (u32 rep{1}; rep < options.benchmark.value_or(1); ++rep) {
      (void)day.parse(buffer.get_span());
    }
    return day.parse(buffer.get_span());
  }();
  time_point t1 = clock_type::now();
  auto const part1_answer = [&] {
    for (u32 rep{1}; rep < options.benchmark.value_or(1); ++rep) {
      (void)day.template solve<false>(parsed);
    }
    return day.template solve<false>(parsed);
  }();
  time_point t2 = clock_type::now();
  auto const part2_answer = [&] {
    if (options.part2) {
      for (u32 rep{1}; rep < options.benchmark.value_or(1); ++rep) {
        (void)day.template solve<true>(parsed, part1_answer);
      }
      return day.template solve<true>(parsed, part1_answer);
    } else {
      return typename CurrentDay::part2_result_t{};
    }
  }();
  time_point t3 = clock_type::now();

  timing_data curr{.parsing = time_in_us(t0, t1), .part1 = time_in_us(t1, t2), .part2 = time_in_us(t2, t3)};
  curr /= options.benchmark.value_or(1);

  data[DayIdx] = report_line{fmt::format("Day {:02}", CurrentDay::number),
                             options.format(part1_answer),
                             options.format(part2_answer),
                             options.format(curr.parsing),
                             options.format(curr.part1),
                             options.format(curr.part2),
                             options.format(curr.total())};

  return curr;
}

std::tuple<timing_data, report_data>
run(run_options const &options) noexcept {
  report_data entries;
  timing_data summary;
  static_for<implemented_days>([]<sz Day>(timing_data& total, report_data& data, run_options const& opts) {
    total += run_one<Day>(data, opts);
  }, summary, entries, options);
  return std::tuple{summary, entries};
}

int
main(int argc, char **argv) {
  run_options options{};
  bool help{false};
  bool error{false};

  for (int c; (c = getopt(argc, argv, "h1Td:p:b:")) != -1;) {
    switch (c) {
    case 'd': {
      u32 const value = static_cast<u32>(strtoul(optarg, NULL, 10));
      if (value == 0 || value > implemented_days) {
        fprintf(stderr, "Option -%c requires day to be implemented.\n", optopt);
        error = true;
      } else {
        options.single = value - 1;
      }
      break;
    }
    case 'p':
      options.precision = static_cast<u32>(atoi(optarg));
      break;
    case '1':
      options.part2 = false;
      break;
    case 'T':
      options.timing = false;
      break;
    case 'b': {
      u32 const value = static_cast<u32>(strtoul(optarg, NULL, 10));
      if (value == 0) {
        fprintf(stderr, "Option -%c requires value to be positive and non-zero.\n", optopt);
        error = true;
      } else {
        options.benchmark = value;
      }
      break;
    }
    case '?':
      if (optopt == 'p' || optopt == 'd') {
        fprintf(stderr, "Option -%c requires an argument.\n", c);
      } else if (isprint(optopt)) {
        fprintf(stderr, "Unknown option `-%c'.\n", c);
      } else {
        fprintf(stderr, "Unknown option character `\\x%x'.\n", c);
      }
      [[fallthrough]];
    default:
      error = true;
      [[fallthrough]];
    case 'h':
      help = true;
    }
  }

  if (help) {
    fmt::print("Advent of Code 2022 (in Modern C++)\n");
    fmt::print("Created by William Killian (willkill07)\n\n");
    fmt::print("Usage: {} [-h | [-1] [-T] [-d <day_num>] [-p <prec>] [-b <times>]] \n\n", argv[0]);
    fmt::print("    -h           show help\n");
    fmt::print("    -d <day_num> run single day\n");
    fmt::print("    -1           only run part 1\n");
    fmt::print("    -T           disable timing\n");
    fmt::print("    -p <prec=2>  precision of timing output\n");
    fmt::print("    -b <times>   benchmark run (repetition amount)\n");
    return EXIT_SUCCESS;
  }
  if (error) {
    return EXIT_FAILURE;
  }

  auto [summary, entries] = run(options);

  print_table(options, entries, summary);
}
