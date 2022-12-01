#include <algorithm>
#include <chrono>
#include <concepts>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/core.h>

#include "Advent.hpp"
#include "FileBackedBuffer.hpp"
#include "FixedString.hpp"
#include "TabularPrint.hpp"
#include "Timing.hpp"

using std::literals::string_literals::     operator""s;
using std::literals::string_view_literals::operator""sv;

struct run_options {
  u32  precision{2};
  bool part2{true};
};

constexpr sz const report_size = 7;
using report_line              = std::array<std::string, report_size>;

std::string as_string([[maybe_unused]] run_options const& options, std::integral auto value) {
  return fmt::format("{0}", value);
}

std::string as_string(run_options const& options, std::floating_point auto value) {
  return fmt::format("{0:.{1}f}", value, options.precision);
}

std::string const& as_string([[maybe_unused]] run_options const& options, std::string const& value) {
  return value;
}

std::string && as_string([[maybe_unused]] run_options const& options, std::string && value) {
  return value;
}

template <sz DayNum> timing_data run_one(const_sz<DayNum>, std::vector<report_line>& data, run_options const& options) {
  using CurrentDay = std::tuple_element_t<DayNum, all_days>;

  file_backed_buffer buffer{fmt::format("input/day{:02}.txt", DayNum)};
  if (not buffer) {
    return {};
  }

  CurrentDay day;

  time_point t0           = clock_type::now();
  auto const parsed       = day.parse(buffer.get_span());
  time_point t1           = clock_type::now();
  auto const part1_answer = day.solve(false_v, parsed);
  time_point t2           = clock_type::now();
  auto const part2_answer = [&] {
    if (options.part2) {
      return day.solve(true_v, parsed, part1_answer);
    } else {
      return typename CurrentDay::part2_result_t{};
    }
  }();
  time_point t3 = clock_type::now();

  timing_data const curr{.parsing = time_in_us(t0, t1), .part1 = time_in_us(t1, t2), .part2 = time_in_us(t2, t3)};

  data.push_back(report_line{fmt::format("Day {:02}", DayNum),
                             as_string(options, part1_answer),
                             as_string(options, part2_answer),
                             as_string(options, curr.parsing),
                             as_string(options, curr.part1),
                             as_string(options, curr.part2),
                             as_string(options, curr.total())});

  return curr;
}

template <typename T, typename U, size_t Len>
std::array<sz, report_size> calculate_widths(std::array<T, report_size> const& header_names,
                                             std::vector<report_line> const&   entries,
                                             std::array<U, Len> const&         summary_data) {

  // initialize to header width
  std::array<sz, report_size> max;
  std::ranges::transform(header_names, std::begin(max), std::mem_fn(&std::string_view::size));

  // adjust for all entries
  for (auto const& entry : entries) {
    for (sz i{0}; i < max.size(); ++i) {
      max[i] = std::max(max[i], entry[i].size());
    }
  }

  // adjust for summary width
  auto const summary_offset = summary_data.size() - 4;
  auto const width_offset   = max.size() - 4;
  for (sz base{0}; base < 4; ++base) {
    max[base + width_offset] = std::max(max[base + width_offset], summary_data[base + summary_offset].size());
  }

  // add padding to each side
  for (sz& col_width : max) {
    col_width += 2;
  }
  return max;
}

void print_table(run_options const& options, std::vector<report_line> const& entries, timing_data const& summary) noexcept {

  std::array const group_names{""sv, "Answers"sv, "Timing (μs)"sv};
  std::array const header_names{"AoC++2022"sv, "Part 1"sv, "Part 2"sv, "Parse"sv, "Part 1"sv, "Part 2"sv, "Total"sv};

  std::array const summary_data{as_string(options, "Summary"s),
                                as_string(options, summary.parsing),
                                as_string(options, summary.part1),
                                as_string(options, summary.part2),
                                as_string(options, summary.total())};

  std::array const max_widths = calculate_widths(header_names, entries, summary_data);

  auto width_for = [&max_widths](auto... inds) {
    return (max_widths[static_cast<sz>(inds)] + ...) + sizeof...(inds) - 1;
  };

  std::array const header_widths{width_for(0), width_for(1, 2), width_for(3, 4, 5, 6)};
  std::array const summary_widths{width_for(0, 1, 2), width_for(3), width_for(4), width_for(5), width_for(6)};

  print_edge<"  ┏━┳━┓">(header_widths);
  print_data<"  ┃ ┃ ┃">(header_widths, group_names);
  print_edge<"┏━╋━┳━╋━┳━┳━┳━┫">(max_widths);
  print_data<"┃ ┃ ┃ ┃ ┃ ┃ ┃ ┃">(max_widths, header_names);
  print_edge<"┣━╋━╋━╋━╋━╋━╋━┫">(max_widths);
  for (auto const& entry : entries) {
    print_data<"┃ ┃ ┃ ┃ ┃ ┃ ┃ ┃">(max_widths, entry);
  }
  print_edge<"┣━┻━┻━╋━╋━╋━╋━┫">(max_widths);
  print_data<"┃ ┃ ┃ ┃ ┃ ┃">(summary_widths, summary_data);
  print_edge<"┗━┻━┻━┻━┻━┛">(summary_widths);
}

int main() {
  run_options              options;
  timing_data              summary;
  std::vector<report_line> entries;

  options.precision = 2;

  [&]<sz... Days>(std::index_sequence<Days...>) {
    summary = (run_one(const_sz<Days + 1>{}, entries, options) += ...);
  }
  (std::make_index_sequence<implemented_days>{});

  print_table(options, entries, summary);
}
