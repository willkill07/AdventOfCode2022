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
)

target_include_directories(days PUBLIC include)
target_link_libraries(days PRIVATE advent_common lib)
