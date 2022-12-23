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
  day19.cpp
  day20.cpp
  day21.cpp
  day22.cpp
  day23.cpp
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_options(days PRIVATE -fsanitize=integer -fsanitize=nullability -fsanitize=implicit-conversion -fsanitize=array-bounds -fsanitize=address -fsanitize-recover=address -fno-omit-frame-pointer)
  target_link_libraries(days PRIVATE -fsanitize=integer -fsanitize=nullability -fsanitize=implicit-conversion -fsanitize=array-bounds -fsanitize=address -fsanitize-recover=address -fno-omit-frame-pointer)
  set_target_properties(days PROPERTIES
    CXX_CLANG_TIDY clang-tidy
    CXX_CPPCHECK "cppcheck;--force")
endif()

target_include_directories(days PUBLIC include)
target_link_libraries(days PRIVATE advent_common lib)
