add_library(days)
target_sources(days
  PRIVATE
  day01.cpp
  day02.cpp
  day03.cpp
  day04.cpp
  day05.cpp
  day06.cpp
  day07.cpp
  day08.cpp
  day09.cpp
  day10.cpp
  day11.cpp
  day12.cpp
  day13.cpp
  day14.cpp
  day15.cpp
  day16.cpp
  day17.cpp
  day18.cpp
)

find_package(OpenMP REQUIRED)

target_include_directories(days PUBLIC include)
target_link_libraries(days PRIVATE advent_common lib)
target_link_libraries(days PUBLIC OpenMP::OpenMP_CXX)
