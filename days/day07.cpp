#include <doctest/doctest.h>

#include "days/day07.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day07, view) {
  using namespace day07;
  // always create root
  auto root = std::make_unique<dir>("/"sv, nullptr);
  auto curr = root.get();

  for (u32 off{7}; off < std::size(view);) {
    if (view.substr(off + 2, 2) == "ls"sv) {
      // command is '$ ls'
      // advance to next line
      off += 5;
      // while we have something in our directory
      while (off < std::size(view) and view[off] != '$') {
        if (view[off] == 'd') {
          // we have a directory
          std::string_view new_dir;
          off += parse<"dir \0\n">(view.substr(off), new_dir);
          curr->dirs.push_back(new dir{new_dir, curr});
        } else {
          // we have a file
          u32 size;
          off += parse<"\0 ">(view.substr(off), size);
          off += view.substr(off).find_first_of('\n') + 1;
          // update directory sizes
          for (auto *d = curr; d != nullptr; d = d->parent) {
            d->size += size;
          }
        }
      }
    } else {
      // command is '$ cd <dir>'
      // advance to <dir>
      off += 5;
      std::string_view dir_name;
      off += parse<"\0\n">(view.substr(off), dir_name);
      // special case of '..'
      if (dir_name == ".."sv) {
        curr = curr->parent;
      } else {
        // search all children, looking for matching dir
        for (auto *node : curr->dirs) {
          if (node->name == dir_name) {
            curr = node;
            break;
          }
        }
      }
    }
  }
  return root;
}

class filter_visitor {
  u32 filtered{0};

public:
  inline void visit(day07::dir *dir) noexcept {
    for (day07::dir *d : dir->dirs) {
      visit(d);
    }
    if (dir->size < 100'000) {
      filtered += dir->size;
    }
  }

  inline u32 answer() const noexcept {
    return filtered;
  }
};

class find_min_visitor {
  u32 lower;
  u32 the_smallest{-1u};

public:
  find_min_visitor() = delete;

  explicit find_min_visitor(u32 target)
      : lower{target} {
  }

  inline void visit(day07::dir *dir) noexcept {
    for (day07::dir *d : dir->dirs) {
      visit(d);
    }
    if (dir->size >= lower && dir->size < the_smallest) {
      the_smallest = dir->size;
    }
  }

  inline u32 answer() const noexcept {
    return the_smallest;
  }
};

PART1_IMPL(Day07, root) {
  filter_visitor v;
  v.visit(root.get());
  return v.answer();
}

PART2_IMPL(Day07, root, part1_answer) {
  u32 const used_space{root->size};
  constexpr u32 const total_space{70'000'000};
  constexpr u32 const needed_free_space{30'000'000};
  u32 const available_space{total_space - used_space};
  u32 const need_to_free{needed_free_space - available_space};
  find_min_visitor m{need_to_free};
  m.visit(root.get());
  return m.answer();
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
