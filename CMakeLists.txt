cmake_minimum_required(VERSION 3.20)
project(AdventOfCode2021 CXX)

set(EXPORT_COMPILE_COMMANDS On)

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        9.1.0
)
FetchContent_MakeAvailable(fmt)

add_executable(advent
    Day01.cpp
    Advent.cpp)

target_compile_features(advent PUBLIC cxx_std_23)
target_compile_options(advent PUBLIC "-Wall" "-Wextra" "-Wpedantic" "-Wconversion" "-Wuninitialized" "-O3" "-march=native")
target_link_libraries(advent PUBLIC fmt::fmt)