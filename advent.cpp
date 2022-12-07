#include <cstdlib>
#include <tuple>
#include <utility>
#include <vector>

#include <unistd.h>

#include <doctest/doctest.h>
#include <fmt/core.h>

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

  file_backed_buffer buffer{fmt::format("input/day{:02}.txt", CurrentDay::number)};
  if (not buffer) {
    return {};
  }

  CurrentDay day;
  time_point t0 = clock_type::now();
  auto repeat = [&](auto &&fn, auto &&...args) {
    for (u32 rep{1}; rep < options.benchmark.value_or(1); ++rep) {
      (void)fn(std::forward<decltype(args)>(args)...);
    }
    return fn(std::forward<decltype(args)>(args)...);
  };
  auto const parsed = repeat([&] {
    return day.parse_input(buffer.get_string_view());
  });
  time_point t1 = clock_type::now();
  auto const part1_answer = repeat([&] {
    return day.part1(parsed);
  });
  time_point t2 = clock_type::now();
  auto const part2_answer = [&]() -> typename CurrentDay::part2_result_t {
    if (options.part2) {
      return repeat([&] {
        return day.part2(parsed, part1_answer);
      });
    } else {
      return {};
    }
  }();
  time_point t3 = clock_type::now();

  timing_data curr{.parsing = time_in_us(t0, t1), .part1 = time_in_us(t1, t2), .part2 = time_in_us(t2, t3)};
  curr /= options.benchmark.value_or(1);
  timing.push_back(curr);

  data[DayIdx] = report_line{fmt::format("Day {:02}", CurrentDay::number),
                             options.format_answer(part1_answer),
                             options.format_answer(part2_answer),
                             options.format(curr.parsing),
                             options.format(curr.part1),
                             options.format(curr.part2),
                             options.format(curr.total())};

  return curr;
}

auto
run(run_options const &options) noexcept {
  using Result = std::tuple<timing_data, report_timing, report_data>;
  return fold<implemented_days>(
      Result{timing_data{}, report_timing{}, implemented_days},
      []<usize Day>(Result &acc, constant_t<Day>, run_options const &opts) {
        std::get<timing_data>(acc) += run_one<Day>(std::get<report_data>(acc), std::get<report_timing>(acc), opts);
      },
      options);
}

int
main(int argc, char **argv) {
  run_options options{};
  bool help{false};
  bool error{false};

  while (true) {
    switch (int const curr_opt{getopt(argc, argv, "htg12TCNMd:p:b:w:")}; curr_opt) {
    case 't':
      return doctest::Context{argc, argv}.run();
      break;
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
    fmt::print(R"AOC_HELP(
Advent of Code 2022 (in Modern C++)
(c) 2022 William Killian

Usage: {} [-h|-t|[-1|-2] [-T|[[-N|-M] [-p <prec>] [-b <times>]] [-C] [-d <day_num>| -g [-w <num>]]]

    -h             show help
    -t             run tests and exit
                   when specified first, other command-line args will be forwarded
                   automatically to doctest (invoke with --help to see options)
    -d <day_num>   run single day
    -1             only show and run part 1
    -2             only show part 2
    -T             suppress timing
    -N             suppress answers
    -C             suppress color output
    -M             mask answers
    -p <prec={}>    precision of timing output
    -b <times>     benchmark run repetition amount
    -g             show graphs
    -w <width>     width of graphs
)AOC_HELP",
               argv[0],
               run_options::default_precision);
    return (error ? EXIT_FAILURE : EXIT_SUCCESS);
  }

  auto [summary, timing, entries] = run(options);

  print(options, entries, summary);

  if (options.graphs) {
    graph_output(options, timing, entries);
  }
  return EXIT_SUCCESS;
}
