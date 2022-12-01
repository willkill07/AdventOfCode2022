cmake_minimum_required(VERSION 3.20)
project(AdventOfCode2021 CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS On)

set(common_flags -Wall -Wextra -Wpedantic -Wconversion -Wuninitialized -O3 -march=native)

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        9.1.0
)
FetchContent_MakeAvailable(fmt)
target_compile_features(fmt PUBLIC cxx_std_23)
target_compile_options(fmt PUBLIC ${common_flags})

add_executable(advent)
add_subdirectory(days)
add_subdirectory(src)
target_include_directories(advent PUBLIC days)
target_include_directories(advent PUBLIC include)
target_compile_features(advent PUBLIC cxx_std_23)
target_compile_options(advent PUBLIC ${common_flags})
target_link_libraries(advent PUBLIC fmt::fmt)