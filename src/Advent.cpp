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

  if (options.single.has_value() && options.single.value() != DayIdx) {
    return {};
  }

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
  run_options options{};
  bool help{false};
  bool error{false};

  for (int c; (c = getopt(argc, argv, "h1td:p:")) != -1;) {
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
    case 't':
      options.timing = true;
      break;
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
    fmt::print("Usage: {} [-h | [-1] [-t] [-d <day_num>] [-p <prec>]] \n\n",
               argv[0]);
    fmt::print("    -h           show help\n");
    fmt::print("    -d <day_num> run single day\n");
    fmt::print("    -1           only run part 1\n");
    fmt::print("    -t           enable timing\n");
    fmt::print("    -p <prec=2>  precision of timing output\n");
    return EXIT_SUCCESS;
  }
  if (error) {
    return EXIT_FAILURE;
  }

  auto [summary, entries] = run(options);

  print_table(options, entries, summary);
}
