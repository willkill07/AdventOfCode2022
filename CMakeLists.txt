cmake_minimum_required(VERSION 3.20)
project(AdventOfCode2021 CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS On)
set(CMAKE_BUILD_TYPE Release)

set(common_flags -Wall -Wextra -Wpedantic -Wconversion -Wuninitialized -Wshadow)
if(NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "AppleClang")
  list(APPEND common_flags -march=native)
endif()

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        9.1.0
)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG        v2.4.9
)
FetchContent_MakeAvailable(fmt doctest)
target_compile_features(fmt PUBLIC cxx_std_23)
target_compile_options(fmt PUBLIC ${common_flags})

add_library(advent_common INTERFACE)
target_compile_features(advent_common INTERFACE cxx_std_23)
target_include_directories(advent_common INTERFACE days include)
target_compile_options(advent_common INTERFACE ${common_flags})
target_link_libraries(advent_common INTERFACE doctest::doctest)

add_library(advent_days INTERFACE)
add_subdirectory(days)
target_link_libraries(advent_days INTERFACE advent_common)

add_executable(advent)
add_subdirectory(src)
target_compile_definitions(advent PRIVATE DOCTEST_CONFIG_DISABLE)
target_link_libraries(advent PUBLIC advent_days fmt::fmt)

enable_testing()
add_executable(advent_test test/runner.cpp)
target_link_libraries(advent_test PUBLIC advent_days)
