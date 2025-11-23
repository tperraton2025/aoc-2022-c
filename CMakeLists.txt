cmake_minimum_required(VERSION 3.10.0)
project(
  AOC_2022_SOLUTIONS
  VERSION 0.1.0
  LANGUAGES C CXX)

enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/cmake)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_SOURCE_DIR}/000)
add_subdirectory(${CMAKE_SOURCE_DIR}/001)
add_subdirectory(${CMAKE_SOURCE_DIR}/002)
add_subdirectory(${CMAKE_SOURCE_DIR}/003)
add_subdirectory(${CMAKE_SOURCE_DIR}/004)
add_subdirectory(${CMAKE_SOURCE_DIR}/005)
add_subdirectory(${CMAKE_SOURCE_DIR}/007)
add_subdirectory(${CMAKE_SOURCE_DIR}/009)
add_subdirectory(${CMAKE_SOURCE_DIR}/099)
