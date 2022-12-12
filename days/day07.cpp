#include <iterator>
#include <numeric>

#include <doctest/doctest.h>

#include "days/day07.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day07, view) {

  std::vector<u32> sizes;
  std::vector<u32> dir_stack;

  for (u64 off{0}; off < std::size(view);) {
    if ('0' <= view[off] and view[off] <= '9') {
      u32 size;
      off += parse<"\0 ">(view.substr(off), size);
      for (auto &dir : dir_stack) {
        dir += size;
      }
    } else if (view.substr(off + 2, 2) == "cd"sv) {
      off += 5; // advance to directory name
      if (view[off] == '.') {
        sizes.push_back(dir_stack.back());
        dir_stack.pop_back();
      } else {
        dir_stack.push_back(0u);
      }
    }
    off += view.substr(off).find_first_of('\n') + 1lu;
  }
  sizes.reserve(std::size(sizes) + std::size(dir_stack));
  std::copy(std::rbegin(dir_stack), std::rend(dir_stack), std::back_insert_iterator(sizes));
  return sizes;
}

PART1_IMPL(Day07, sizes) {
  return std::accumulate(std::begin(sizes), std::end(sizes), 0u, [](u32 acc, u32 s) {
    return acc + (s < 100'000 ? s : 0u);
  });
}

constexpr u32 const total_space{70'000'000};
constexpr u32 const needed_free_space{30'000'000};

PART2_IMPL(Day07, sizes, part1_answer) {
  u32 const used_space{sizes.back()};
  u32 const available_space{total_space - used_space};
  u32 const need_to_free{needed_free_space - available_space};
  return std::accumulate(std::begin(sizes), std::end(sizes), -1u, [need_to_free](u32 best_fit, u32 s) {
    return ((s >= need_to_free && s < best_fit) ? s : best_fit);
  });
}

INSTANTIATE_TEST(Day07,
                 R"(
$ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k
)"sv.substr(1),
                 95437u,
                 24933642u)
