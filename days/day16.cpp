#include "days/day16.hpp"
#include "parsing.hpp"

#include <algorithm>

PARSE_IMPL(Day16, view) {

  using day16::T;

  // mapping of all nodes (temporary)
  typename day16::vector_t id_map;
  // list of all IDs with flow (temporary)
  typename day16::compact_vector_t with_flow;
  // flow amount for valves
  typename day16::compact_vector_t flow_amount;
  // distance matrix (temporary)
  typename day16::matrix_t dist(day16::MAX_VALVES * day16::MAX_VALVES, std::numeric_limits<u16>::max());

  auto get_id = [](auto &map, char a, char b) {
    T const val{static_cast<T>(a << 8 | b)};
    if (auto iter = std::find(std::begin(map), std::end(map), val); iter == std::end(map)) {
      map.push(val);
      return static_cast<T>(std::size(map) - 1);
    } else {
      return static_cast<T>(std::distance(std::begin(map), iter));
    }
  };

  // by line
  for (u32 off{6}; off < std::size(view); off += 6) {
    T const valve_id{get_id(id_map, view[off], view[off + 1])};
    off += 17;
    T rate{static_cast<T>(view[off] - '0')};
    if (view[off + 1] != ';') {
      ++off;
      rate = static_cast<T>(10 * rate) + static_cast<T>(view[off] - '0');
    }
    if (rate > 0) {
      with_flow.push(valve_id);
      flow_amount.push(rate);
    }
    off += 24;
    if (view[off] != ' ') {
      ++off;
    }
    while (view[off] == ' ') {
      ++off;
      T const adj{get_id(id_map, view[off], view[off + 1])};
      dist[static_cast<u32>(day16::MAX_VALVES * valve_id + adj)] = 1;
      off += 3;
    }
  }

  // u32 const N{std::size(id_map)};

  // Floyd-Warshall Algorithm -- ASSP
  // - note: `std::size(id_map)` should be used here, however,
  //         the compiler LOVES compile-time known values for
  //         better loop unrolling and autovectorization
  for (u32 k{0}; k < day16::MAX_VALVES; ++k) {
    for (u32 i{0}; i < day16::MAX_VALVES; ++i) {
      for (u32 j{0}; j < day16::MAX_VALVES; ++j) {
        dist[day16::MAX_VALVES * i + j] =
            std::min(dist[day16::MAX_VALVES * i + j],
                     static_cast<T>(dist[day16::MAX_VALVES * i + k] + dist[day16::MAX_VALVES * k + j]));
      }
    }
  }

  i32 const AA{get_id(id_map, 'A', 'A')};
  flow_amount.push(0);

  // reduced distance matrix -- only considers those with flow (or 'AA')
  typename day16::compact_matrix_t connected;
  for (auto &&src : with_flow) {
    for (auto &&dst : with_flow) {
      connected.push(dist[static_cast<u32>(day16::MAX_VALVES * src + dst)]);
    }
    connected.push(dist[static_cast<u32>(day16::MAX_VALVES * src + AA)]);
  }
  for (auto &&dst : with_flow) {
    connected.push(dist[static_cast<u32>(day16::MAX_VALVES * AA + dst)]);
  }
  connected.push(dist[static_cast<u32>(day16::MAX_VALVES * AA + AA)]);

  return {std::move(flow_amount), std::move(connected)};
}

constexpr u32 const MAXN{day16::MAX_WORKING_VALVES};

void
visit(i32 (&dp)[1 << MAXN],
      day16::compact_vector_t const &rates,
      day16::compact_matrix_t const &dist,
      u32 curr,
      i32 time_remaining,
      u32 state,
      i32 flow) noexcept {

  dp[state] = std::max(dp[state], flow);

  // check possible places
  for (u32 next{0}; next < std::size(rates); ++next) {
    if (rates[next] == 0) {
      continue;
    }

    auto const new_time_remaining{time_remaining - dist[MAXN * curr + next] - 1};

    if (new_time_remaining > 0 and not((1 << next) & state)) {
      u32 const new_state{state | (1 << next)};
      i32 const new_flow{flow + new_time_remaining * rates[next]};
      visit(dp, rates, dist, next, new_time_remaining, new_state, new_flow);
    }
  }
}

SOLVE_IMPL(Day16, Part2, data, unused_part1) {
  i32 dp[1 << MAXN];
  std::fill_n(std::addressof(dp[0]), sizeof(dp) / sizeof(dp[0]), 0);

  u32 const start{std::size(data.flow) - 1};
  i32 const time{Part2 ? 26 : 30};
  u32 const mask{0u};

  visit(dp, data.flow, data.dist, start, time, mask, 0);

  if constexpr (Part2) {
    i32 max{0};
    // the dynamic schedule here is important since we are doing upper-triangular
#pragma omp parallel for reduction(max : max) schedule(dynamic)
    for (u32 m1 = 0; m1 < std::size(dp); ++m1) {
      for (u32 m2 = m1 + 1; m2 < std::size(dp); ++m2) {
        if (not(m1 & m2)) {
          max = std::max(max, dp[m1] + dp[m2]);
        }
      }
    }
    return max;
  } else {
    return *std::max_element(std::begin(dp), std::end(dp));
  }
}

INSTANTIATE(Day16);

INSTANTIATE_TEST(Day16,
                 R"(
Valve AA has flow rate=0; tunnels lead to valves DD, II, BB
Valve BB has flow rate=13; tunnels lead to valves CC, AA
Valve CC has flow rate=2; tunnels lead to valves DD, BB
Valve DD has flow rate=20; tunnels lead to valves CC, AA, EE
Valve EE has flow rate=3; tunnels lead to valves FF, DD
Valve FF has flow rate=0; tunnels lead to valves EE, GG
Valve GG has flow rate=0; tunnels lead to valves FF, HH
Valve HH has flow rate=22; tunnel leads to valve GG
Valve II has flow rate=0; tunnels lead to valves AA, JJ
Valve JJ has flow rate=21; tunnel leads to valve II
)"sv.substr(1),
                 1651,
                 1707)
