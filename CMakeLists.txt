cmake_minimum_required(VERSION 3.10.0)
project(
  AOC_2022_SOLUTIONS
  VERSION 0.1.0
  LANGUAGES C CXX)

enable_testing()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/2022) 
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/2025)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/099)


