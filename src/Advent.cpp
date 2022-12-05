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
      (void)day.parse(buffer.get_string_view());
    }
    return day.parse(buffer.get_string_view());
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
                             options.format_answer(part1_answer),
                             options.format_answer(part2_answer),
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
  static_for<implemented_days>(
      []<sz Day>(timing_data &total, report_data &data, run_options const &opts) {
        total += run_one<Day>(data, opts);
      },
      summary,
      entries,
      options);
  return std::tuple{summary, entries};
}

int
main(int argc, char **argv) {
  run_options options{};
  bool help{false};
  bool error{false};

  while (true) {
    switch (int const curr_opt{getopt(argc, argv, "h12TCNMd:p:b:")}; curr_opt) {
    case -1:
      goto option_parsing_done;
    case 'd': {
      u32 const value = static_cast<u32>(strtoul(optarg, NULL, 10));
      if (value == 0 || value > implemented_days) {
        fprintf(stderr, "Option -%c requires day to be implemented.\n", curr_opt);
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
    case '2':
      options.part1 = false;
      break;
    case 'T':
      options.timing = false;
      break;
    case 'C':
      options.colorize = false;
      break;
    case 'N':
      options.answers = false;
      break;
    case 'M':
      options.mask = true;
      break;
    case 'b': {
      u32 const value = static_cast<u32>(strtoul(optarg, NULL, 10));
      if (value == 0) {
        fprintf(stderr, "Option -%c requires value to be positive and non-zero.\n", curr_opt);
        error = true;
      } else {
        options.benchmark = value;
      }
      break;
    }
    case '?':
      if (optopt == 'p' || optopt == 'd') {
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      } else if (isprint(optopt)) {
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      } else {
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      }
      [[fallthrough]];
    default:
      error = true;
      [[fallthrough]];
    case 'h':
      help = true;
    }
  }
option_parsing_done:

  error = error or not options.validate();

  if (help or error) {
    fmt::print(R"AOC_HELP(
Advent of Code 2022 (in Modern C++)
(c) 2022 William Killian

Usage: {} [-h|[-1|-2] [-T|[[-N|-M] [-p <prec>] [-b <times>]] [-C] [-d <day_num>]]

    -h             show help
    -d <day_num>   run single day
    -1             only show and run part 1
    -2             only show part 2
    -T             suppress timing
    -N             suppress answers
    -C             suppress color output
    -M             mask answers
    -p <prec={}>    precision of timing output
    -b <times>     benchmark run repetition amount
)AOC_HELP",
               argv[0],
               run_options::default_precision);
    return (error ? EXIT_FAILURE : EXIT_SUCCESS);
  }

  auto [summary, entries] = run(options);

  print_table(options, entries, summary);
}
