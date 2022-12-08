cmake_minimum_required(VERSION 3.20)
project(AdventOfCode2022 CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS On)

if(DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
  add_compile_options(-fsanitize=address -g)
  add_link_options(-fsanitize=address -g)
else()
  set(CMAKE_BUILD_TYPE Release)
endif()

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

add_library(advent_common INTERFACE)
target_compile_features(advent_common INTERFACE cxx_std_23)
target_include_directories(advent_common INTERFACE days include)
target_compile_options(advent_common INTERFACE ${common_flags})
target_link_libraries(advent_common INTERFACE doctest::doctest)

add_subdirectory(days)
add_subdirectory(lib)

add_executable(advent)
target_sources(advent PUBLIC advent.cpp)
target_compile_definitions(advent PRIVATE DOCTEST_CONFIG_IMPLEMENT)
target_link_libraries(advent PRIVATE days lib)