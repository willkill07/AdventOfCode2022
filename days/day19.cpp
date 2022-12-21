#include <algorithm>
#include <numeric>
#include <thread>
#include <utility>

#include "days/day19.hpp"
#include "owning_span.hpp"
#include "parsing.hpp"
#include "types.hpp"

namespace {

constexpr u32 const Ore{std::to_underlying(day19::resource::ore)};
constexpr u32 const Clay{std::to_underlying(day19::resource::clay)};
constexpr u32 const Obsidian{std::to_underlying(day19::resource::obsidian)};
constexpr u32 const Geode{std::to_underlying(day19::resource::geode)};

[[nodiscard]] constexpr inline i32
ceildiv(i32 num, i32 den) noexcept {
  return (num + den - 1) / den;
}

[[nodiscard]] constexpr inline u32
time_until_available(auto const &cost_of_robot, auto const &resources_available, auto const &robots) noexcept {
  i32 max_time{0u};
  for (u32 i{0u}; i < 3u; i++) {
    if (cost_of_robot[i] != 0) {
      if (auto const produced_by_robot{robots[i]}; produced_by_robot == 0) {
        return 0xFFu;
      } else if (cost_of_robot[i] > resources_available[i]) {
        auto const needed{cost_of_robot[i] - resources_available[i]};
        max_time = std::max(max_time, ceildiv(needed, produced_by_robot));
      }
    }
  }
  return as<u32>(max_time);
}

[[nodiscard]] constexpr inline day19::robot
max_for(day19::blueprint const &blueprint) {
  day19::robot max_robots{0, 0, 0};
  for (u32 j{0}; j < 3; ++j) {
    for (auto &curr : blueprint) {
      max_robots[j] = std::max(max_robots[j], curr[j]);
    }
  }
  return max_robots;
}

constexpr std::array<u8, 4> const empty{0, 0, 0, 0};
constexpr std::array<u8, 4> const init{1, 0, 0, 0};

} // namespace

PARSE_IMPL(Day19, view) {
  using day19::resource;
  day19::blueprints result;

  constexpr fixed_string LINE{"Blueprint \0: "
                              "Each ore robot costs \1 ore. "
                              "Each clay robot costs \2 ore. "
                              "Each obsidian robot costs \3 ore and \4 clay. "
                              "Each geode robot costs \5 ore and \6 obsidian.\n"};

  for (usize off{0}; off < std::size(view);) {
    day19::blueprint curr;
    std::fill_n(std::addressof(curr[0][0]), std::size(curr) * std::size(curr[0]), 0);
    [[maybe_unused]] u32 id; // swallow the id and never use it again
    off += parse<LINE>(view.substr(off),
                       id,
                       curr[Ore][Ore],
                       curr[Clay][Ore],
                       curr[Obsidian][Ore],
                       curr[Obsidian][Clay],
                       curr[Geode][Ore],
                       curr[Geode][Obsidian]);
    result.push(curr);
  }
  return result;
}

SOLVE_IMPL(Day19, Part2, blueprints, part1_answer) {

  u32 const upper{Part2 ? std::min(std::size(blueprints), 3u) : std::size(blueprints)};

  owning_span<u32, 30> scores(upper, 0u);
  owning_span<std::thread, 30> threads;
  for (u32 i{0u}; i < upper; ++i) {
    threads.push(std::thread(
        [&](day19::blueprint const &blueprint, u32 &score) {
          day19::robot const max_limit = max_for(blueprint);

          u32 max_geodes{0u};

          // recursive lambda time!
          auto step = [&](auto &self,
                          std::array<u8, 4> const &resources,
                          std::array<u8, 4> const &robots,
                          u32 time) noexcept -> u32 {
            u32 planned{resources[Geode] + robots[Geode] * time};

            if (planned + (time * time - time) / 2u <= max_geodes) {
              return 0;
            }

            // try to construct each robot
            for (u32 robot_idx{0}; robot_idx < 4; robot_idx++) {
              // never create more of a resource than what we can possible use (except for Geode)
              if (robot_idx == Geode or max_limit[robot_idx] > robots[robot_idx]) {
                // if time for resources + creation time is within limit, then explore
                if (u32 const wait{time_until_available(blueprint[robot_idx], resources, robots) + 1u}; time > wait) {
                  // evolve resources
                  std::array<u8, 4> const next_resources{
                      as<u8>(resources[Ore] + robots[Ore] * wait - blueprint[robot_idx][Ore]),
                      as<u8>(resources[Clay] + robots[Clay] * wait - blueprint[robot_idx][Clay]),
                      as<u8>(resources[Obsidian] + robots[Obsidian] * wait - blueprint[robot_idx][Obsidian]),
                      as<u8>(resources[Geode] + robots[Geode] * wait)};
                  // update robot count
                  std::array<u8, 4> next_robots{robots};
                  ++next_robots[robot_idx];
                  // continue our DFS
                  planned = std::max(planned, self(self, next_resources, next_robots, time - wait));
                }
              }
            }
            // update max geodes after exploring subtree
            max_geodes = std::max(max_geodes, planned);
            return planned;
          };

          // simulate + write out score
          score = step(step, empty, init, (Part2 ? 32u : 24u));
        },
        std::cref(blueprints[i]),
        std::ref(scores[i])));
  }
  for (auto &t : threads) {
    t.join();
  }

  if constexpr (Part2) {
    return std::accumulate(std::begin(scores), std::begin(scores) + upper, 1u, std::multiplies<>{});
  } else {
    return std::accumulate(std::begin(scores), std::end(scores), 0u, [i = 0u](u32 acc, u32 s) mutable noexcept {
      return acc + (++i * s);
    });
  }
}

INSTANTIATE(Day19);

INSTANTIATE_TEST(Day19,
                 R"(
Blueprint 1: Each ore robot costs 4 ore. Each clay robot costs 2 ore. Each obsidian robot costs 3 ore and 14 clay. Each geode robot costs 2 ore and 7 obsidian.
Blueprint 2: Each ore robot costs 2 ore. Each clay robot costs 3 ore. Each obsidian robot costs 3 ore and 8 clay. Each geode robot costs 3 ore and 12 obsidian.
)"sv.substr(1),
                 33,
                 3472)
