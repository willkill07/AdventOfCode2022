#include <chrono>
#include <iostream>
#include <optional>

#include <fmt/core.h>
#include <fmt/color.h>
#include <ratio>

#include "Advent.hpp"

inline double time_in_us(auto start, auto stop) noexcept {
  return std::chrono::duration<double, std::micro>(stop - start).count();
}

int main() {
  double total_parsing{0.0};
  double total_part1{0.0};
  double total_part2{0.0};
  double total{0.0};

  std::string const AOC = 
    fmt::format(fg(fmt::terminal_color::bright_green), " Ao") +
    fmt::format(fg(fmt::terminal_color::bright_yellow) | fmt::emphasis::bold, "C++") +
    fmt::format(fg(fmt::terminal_color::bright_green), "2022 ");
  
  fmt::print(" {0: ^11}┏{0:━^32}━{0:━^32}┳{0:━^51}┓\n", "");
  fmt::print(" {0: ^11}┃{1: ^65}┃{2: ^51}┃\n", "", "Answers", "Timing (μs)");
  fmt::print("┏{0:━^11}╋{0:━^32}┯{0:━^32}╋{0:━^12}┯{0:━^12}┯{0:━^12}┯{0:━^12}┫\n", "");
  fmt::print("┃{0: ^38}┃{1: ^32}│{2: ^32}┃{3: ^12}│{4: ^12}│{5: ^12}│{6: ^12}┃\n", AOC, "Part 1", "Part 2", "Parse", "Part 1", "Part 2", "Total");
  fmt::print("┣{0:━^11}╋{0:━^32}┿{0:━^32}╋{0:━^12}┿{0:━^12}┿{0:━^12}┿{0:━^12}┫\n", "");

  [&]<sz... Is>(std::index_sequence<Is...>) {
    (([&]<sz I>(const_sz<I>) {
       constexpr auto const day_num = I + 1;
       FILE *file =
           fopen(fmt::format("input/day{:02}.txt", day_num).c_str(), "r");
       if (file == nullptr) {
         std::cerr << fmt::format(
             "Day{0:02}: cannot read input file input/day{0:02}.txt\n",
             day_num);
         return;
       }

       using CurrentDay = std::tuple_element_t<I, all_days>;

       CurrentDay day;

       auto t0 = std::chrono::steady_clock::now();
       auto parsed = day.parse(file);
       auto t1 = std::chrono::steady_clock::now();
       auto part1_answer = day.solve(false_v, parsed);
       auto t2 = std::chrono::steady_clock::now();
       auto part2_answer = day.solve(true_v, parsed, part1_answer);
       auto t3 = std::chrono::steady_clock::now();

       double const parse_time = time_in_us(t0, t1);
       double const part1_time = time_in_us(t1, t2);
       double const part2_time = time_in_us(t2, t3);
       double const total_time = time_in_us(t0, t3);

       total_parsing += parse_time;
       total_part1 += part1_time;
       total_part2 += part2_time;
       total += total_time;

       fmt::print("┃{:^11}┃{:^32}│{:^32}┃ {:>10.2f} │ {:>10.2f} │ {:>10.2f} │ {:>10.2f} ┃\n", fmt::format("Day {:02}", day_num), part1_answer, part2_answer, parse_time, part1_time, part2_time, total_time);

       fclose(file);
     }(const_sz<Is>{})),
     ...);
  }
  (std::make_index_sequence<implemented_days>{});

  fmt::print("┣{0:━^11}╋{0:━^32}┷{0:━^32}╋{0:━^12}┿{0:━^12}┿{0:━^12}┿{0:━^12}┫\n", "");
  fmt::print("┃{: ^11}┃ {:^30}   {:^30} ┃ {:>10.2f} │ {:>10.2f} │ {:>10.2f} │ {:>10.2f} ┃\n", "Summary", "", "", total_parsing, total_part1, total_part2, total);
  fmt::print("┗{0:━^11}┻{0:━^32}━{0:━^32}┻{0:━^12}┷{0:━^12}┷{0:━^12}┷{0:━^12}┛\n", "");

}
