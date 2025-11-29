cmake_minimum_required(VERSION 3.10.0)
project(
  LIB_AOC_HELPERS
  VERSION 0.1.0
  LANGUAGES C CXX)
enable_language(C)

set(2D_ENGINE_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/2d_engine/tools.c
    ${CMAKE_CURRENT_SOURCE_DIR}/2d_engine/input.c
    ${CMAKE_CURRENT_SOURCE_DIR}/2d_engine/cursor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/2d_engine/engine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/2d_engine/objects.c)
add_library(
  libaochelpers
  aoc_solution.c
  aoc_linked_list.c
  aoc_parser.c
  aoc_stack.c
  aoc_ring_buffer.c
  aoc_tree.c
  ${2D_ENGINE_SRCS})
set(CMAKE_BUILD_TYPE DEBUG)

target_link_libraries(libaochelpers m)

enable_testing()
add_subdirectory(tests)

target_include_directories(
  libaochelpers
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/2d_engine/inc
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
