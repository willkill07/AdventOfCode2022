#include "TabularPrint.hpp"

#include <array>
#include <cstdio>
#include <functional>
#include <string_view>
#include <utility>

#include <fmt/core.h>

#include "FixedString.hpp"
#include "Types.hpp"

template <fixed_string String, sz Width>
void print_edge(std::array<sz, Width> const &widths,
                std::array<bool, Width> const &masks) {
  [&]<sz... I>(std::integer_sequence<sz, I...>) {
    (
        [&] {
          if (masks[I] and widths[I] > 0) {
            fmt::print("{}", String[2 * I]);
            auto const fmt_str =
                fmt::format("{{:{}^{}}}", String[2 * I + 1], widths[I]);
            fmt::vprint(fmt_str, fmt::make_format_args(""));
          }
        }(),
        ...);
  }
  (std::make_integer_sequence<sz, Width>{});
  fmt::print("{}", String[String.printable_size - 1]);
  fmt::print("\n");
}

template <fixed_string String, sz Width, sz Data, sz Mask, typename T>
void print_data(std::array<sz, Width> const &widths,
                std::array<T, Data> const &data,
                std::array<bool, Mask> const &masks) {
  static_assert(Data == Width, "data array does not match width array");
  static_assert(Mask == Width, "mask array does not match width array");
  [&]<sz... I>(std::integer_sequence<sz, I...>) {
    (
        [&] {
          if (masks[I] and widths[I] > 0) {
            fmt::print("{}", String[2 * I]);
            auto const fmt_str =
                fmt::format("{{:{}^{}}}", String[2 * I + 1], widths[I]);
            fmt::vprint(fmt_str, fmt::make_format_args(data[I]));
          }
        }(),
        ...);
  }
  (std::make_integer_sequence<sz, Width>{});
  fmt::print("{}", String[String.printable_size - 1]);
  fmt::print("\n");
}

template <typename T, typename U, size_t Len>
std::array<sz, report_size>
calculate_widths(std::array<T, report_size> const &header_names,
                 report_data const &entries,
                 std::array<U, Len> const &summary_data,
                 std::array<bool, report_size> const &mask) {

  // initialize to header width
  std::array<sz, report_size> max;
  std::transform(std::begin(header_names), std::end(header_names),
                 std::begin(max), std::mem_fn(&std::string_view::size));

  // adjust for all entries
  for (auto const &entry : entries) {
    for (sz i{0}; i < max.size(); ++i) {
      max[i] = std::max(max[i], entry[i].size());
    }
  }

  // adjust for summary width
  auto const summary_offset = summary_data.size() - 4;
  auto const width_offset = max.size() - 4;
  for (sz base{0}; base < 4; ++base) {
    max[base + width_offset] = std::max(
        max[base + width_offset], summary_data[base + summary_offset].size());
  }

  // correct for padding/mask
  for (sz idx{0}; idx < report_size; ++idx) {
    if (mask[idx]) {
      max[idx] += 2;
    } else {
      max[idx] = 0;
    }
  }

  return max;
}

void print_table(run_options const &options, report_data const &entries,
                 timing_data const &summary) {

  using std::literals::string_literals::operator""s;
  using std::literals::string_view_literals::operator""sv;

  std::array const group_names{""sv, "Answers"sv, "Timing (μs)"sv};
  std::array const header_names{"AoC++2022"sv, " Part 1 "sv, " Part 2 "sv,
                                "Parse"sv,     "Part 1"sv,   "Part 2"sv,
                                "Total"sv};

  std::array const summary_data{
      options.format("Summary"s), options.format(summary.parsing),
      options.format(summary.part1), options.format(summary.part2),
      options.format(summary.total())};

  std::array const header_mask{true, true, options.timing};
  std::array const content_mask{true,           true,
                                options.part2,  options.timing,
                                options.timing, options.part2 && options.timing,
                                options.timing};
  std::array const summary_mask{true, options.timing, options.timing,
                                options.timing, options.timing};

  std::array const max_widths =
      calculate_widths(header_names, entries, summary_data, content_mask);

  auto width = [&](auto... inds) -> sz {
    auto const sum = (max_widths[static_cast<sz>(inds)] + ...);
    auto const count = (content_mask[static_cast<sz>(inds)] + ...);
    if (count > 0) {
      return sum + static_cast<sz>(count - 1);
    } else {
      return 0;
    }
  };

  std::array const header_widths{width(0), width(1, 2), width(3, 4, 5, 6)};

  std::array const summary_widths{width(0, 1, 2), width(3), width(4), width(5),
                                  width(6)};

  print_edge<"  ╭─┬─╮">(header_widths, header_mask);
  print_data<"  │ │ │">(header_widths, group_names, header_mask);
  print_edge<"╭─┼─┬─┼─┬─┬─┬─┤">(max_widths, content_mask);
  print_data<"│ │ │ │ │ │ │ │">(max_widths, header_names, content_mask);
  print_edge<"├─┼─┼─┼─┼─┼─┼─┤">(max_widths, content_mask);
  for (auto const &entry : entries) {
    print_data<"│ │ │ │ │ │ │ │">(max_widths, entry, content_mask);
  }
  if (options.timing) {
    print_edge<"├─┴─┴─┼─┼─┼─┼─┤">(max_widths, content_mask);
    print_data<"│ │ │ │ │ │">(summary_widths, summary_data, summary_mask);
    print_edge<"╰─┴─┴─┴─┴─╯">(summary_widths, summary_mask);
  } else {
    print_edge<"╰─┴─┴─┴─┴─┴─┴─╯">(max_widths, content_mask);
  }
}
