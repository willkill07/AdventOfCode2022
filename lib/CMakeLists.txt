add_library(lib)

target_sources(lib
  PRIVATE
  file_backed_buffer.cpp
  options.cpp
  table.cpp
)

target_include_directories(lib PUBLIC include)
target_link_libraries(lib PUBLIC fmt::fmt advent_common)