#include <cstdlib>
#include <tuple>
#include <utility>
#include <vector>

#include <unistd.h>

#ifndef DOCTEST_CONFIG_DISABLE 
#include <doctest/doctest.h>
#endif

#include <fmt/chrono.h>
#include <fmt/compile.h>
#include <fmt/core.h>

#include "chart/calculator.hpp"
#include "chart/chart.hpp"
#include "days/advent_days.hpp"
#include "file_backed_buffer.hpp"
#include "fixed_string.hpp"
#include "graph.hpp"
#include "meta/utils.hpp"
#include "options.hpp"
#include "table.hpp"
#include "timing.hpp"
#include "types.hpp"

template <usize DayIdx>
timing_data
run_one(report_data &data, report_timing &timing, run_options const &options) {
  using CurrentDay = std::tuple_element_t<DayIdx, all_days>;

  if (options.single.has_value() && options.single.value() != DayIdx) {
    return {};
  }

  file_backed_buffer buffer{fmt::format(FMT_COMPILE("input/day{:02}.txt"), CurrentDay::number)};
  if (not buffer) {
    return {};
  }

  CurrentDay day;
  u32 const repetitions = options.benchmark.value_or(1);
  std::string_view const view = buffer.get_string_view();

  timing_data curr;

  time_point t0 = clock_type::now();

  for (u32 rep{1}; rep < repetitions; ++rep) {
    (void)day.parse_input(view);
  }
  auto parsed = day.parse_input(view);
  
  time_point t1 = clock_type::now();
  
  if (options.part1) { 
    for (u32 rep{1}; rep < repetitions; ++rep) {
      (void)day.part1(parsed);
    }
  }
  auto const part1_answer = day.part1(parsed);
  
  time_point t2 = clock_type::now();
  
  if (options.part2) {
    for (u32 rep{1}; rep < repetitions; ++rep) {
      (void)day.part2(parsed, part1_answer);
    }
    auto const part2_answer = day.part2(parsed, part1_answer);

    time_point t3 = clock_type::now();
    if (options.answers) {
      data[DayIdx][std::to_underlying(index::part2_answer)] = options.format_answer(part2_answer);
    }
    curr.part2 = time_in_us(t2, t3);
  }

  data[DayIdx][std::to_underlying(index::day)] = fmt::format(FMT_COMPILE("Day {:02}"), CurrentDay::number);
  if (options.answers and options.part1) {
    data[DayIdx][std::to_underlying(index::part1_answer)] = options.format_answer(part1_answer);
  }

  if (options.timing) {
    curr.parsing = time_in_us(t0, t1);
    curr.part1 = time_in_us(t1, t2);
    curr /= options.benchmark.value_or(1);
    timing[DayIdx] = curr;
  }

  return curr;
}

auto
run(run_options const &options) noexcept {
  using Result = std::tuple<timing_data, report_timing, report_data>;
  auto result = fold<implemented_days>(
      Result{timing_data{}, implemented_days, implemented_days},
      []<usize Day>(Result &acc, constant_t<Day>, run_options const &opts) {
        std::get<timing_data>(acc) += run_one<Day>(std::get<report_data>(acc), std::get<report_timing>(acc), opts);
      },
      options);

  report_timing &times = std::get<report_timing>(result);
  report_data &data = std::get<report_data>(result);
  if (options.visual) {
    chart::calculator<double, 8> parse_time, part1_time, part2_time, total_time;
    for (auto const &t : times) {
      parse_time.add_sample(t.parsing);
      part1_time.add_sample(t.part1);
      part2_time.add_sample(t.part2);
      total_time.add_sample(t.total());
    }
    auto const width = options.precision.value_or(run_options::default_bar_width);
    for (usize i{0}; i < implemented_days; ++i) {
      data[i][std::to_underlying(index::parse_time)] = chart::row(parse_time.length_for(times[i].parsing, width));
      data[i][std::to_underlying(index::part1_time)] = chart::row(part1_time.length_for(times[i].part1, width));
      data[i][std::to_underlying(index::part2_time)] = chart::row(part2_time.length_for(times[i].part2, width));
      data[i][std::to_underlying(index::total_time)] = chart::row(total_time.length_for(times[i].total(), width));
    }
  } else {
    for (usize i{0}; i < std::size(times); ++i) {
      data[i][std::to_underlying(index::parse_time)] = options.format(times[i].parsing);
      data[i][std::to_underlying(index::part1_time)] = options.format(times[i].part1);
      data[i][std::to_underlying(index::part2_time)] = options.format(times[i].part2);
      data[i][std::to_underlying(index::total_time)] = options.format(times[i].total());
    }
  }
  return result;
}

void
minimal_run(bool quiet) {
  time_point start{clock_type::now()};
  static_for<implemented_days>([&]<usize DayId>(constant_t<DayId>) {
    using CurrentDay = std::tuple_element_t<DayId, all_days>;
    CurrentDay day;
    file_backed_buffer buffer{fmt::format(FMT_COMPILE("input/day{:02}.txt"), CurrentDay::number)};
    auto const parsed = day.parse_input(buffer.get_string_view());
    auto const part1 = day.part1(parsed);
    (void)day.part2(parsed, part1);
  });
  if (not quiet) {
    fmt::print(FMT_COMPILE("{}\n"), (clock_type::now() - start));
  }
}

int
main(int argc, char **argv) {
  run_options options{};
  bool help{false};
  bool error{false};
  bool quiet{true};

  while (true) {
#ifndef DOCTEST_CONFIG_DISABLE 
    switch (int const curr_opt{getopt(argc, argv, "Qmhtgv12TCNMd:p:b:w:")}; curr_opt) {
    case 't':
      return doctest::Context{argc, argv}.run();
      break;
#else
    switch (int const curr_opt{getopt(argc, argv, "Qmhgv12TCNMd:p:b:w:")}; curr_opt) {
#endif
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
    case 'Q':
      quiet = false;
      break;
    case 'm':
      minimal_run(quiet);
      exit(0);
    case 'v':
      options.visual = true;
      break;
    case 'g':
      options.graphs = true;
      break;
    case 'w': {
      u32 const value = static_cast<u32>(strtoul(optarg, NULL, 10));
      if (value == 0) {
        fprintf(stderr, "Option -%c requires value to be positive and non-zero.\n", curr_opt);
        error = true;
      } else {
        options.graph_width = value;
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
    fmt::print(FMT_COMPILE(R"AOC_HELP(
Advent of Code 2022 (in Modern C++)
(c) 2022 William Killian

Usage: {} [-h|-t|[-Q] -m|[-1|-2] [-T|[[-N|-M] [-p <prec>] [-b <times>]] [-C] [-d <day_num>| -g [-w <num>]]]

    -h             show help
    -t             run tests and exit (if compiled with support)
                   when specified first, other command-line args will be forwarded
                   automatically to doctest (invoke with --help to see options)

    -m             minimal run
    -Q             noisy mode for minimal run

    -b <times>     benchmark run repetition amount
    -d <day_num>   run single day
    -1             only show and run part 1
    -2             only show part 2

    -T             suppress timing
    -N             suppress answers
    -M             mask answers
    
    -C             suppress color output
    -v             visual mode (show bars instead of numbers for timing)
    -p <prec={}>    precision of timing output
                   for visual mode, this is the bar width (={})
    -g             show graphs
    -w <width={}>  width of graphs
)AOC_HELP"),
               argv[0],
               run_options::default_precision,
               run_options::default_bar_width,
               run_options::default_graph_width);
    return (error ? EXIT_FAILURE : EXIT_SUCCESS);
  }

  auto [summary, timing, entries] = run(options);

  print(options, entries, summary);

  if (options.graphs) {
    graph_output(options, timing, entries);
  }
  return EXIT_SUCCESS;
}
