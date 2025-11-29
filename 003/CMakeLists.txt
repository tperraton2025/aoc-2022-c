cmake_minimum_required(VERSION 3.10.0)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})

project(
  ${ProjectId}
  VERSION 0.1.0
  LANGUAGES C CXX)

add_executable(${ProjectId} main.c part1.c part2.c)
set_property(TARGET ${ProjectId} PROPERTY C_STANDARD 11)
target_include_directories(${ProjectId} PRIVATE ${CMAKE_SOURCE_DIR}
                                                ${CMAKE_SOURCE_DIR}/../lib/inc)

target_link_libraries(${ProjectId} libaochelpers)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

set_target_properties(
  ${ProjectId}
  PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_definitions(-DCONFIG_DAY="${ProjectId}")

# unit tests
include(CTest)
add_test(
  NAME ${ProjectId}test
  COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bin/${ProjectId}"
          "${CMAKE_CURRENT_SOURCE_DIR}/data/test1.txt" --exe "${ProjectId}test")
add_test(NAME ${ProjectId}input
         COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bin/${ProjectId}"
                 "./data/input.txt" --exe "${ProjectId}test")

add_aoc_day_test_regex(${ProjectId}test "157" "70")
add_aoc_day_test_regex(${ProjectId}input "7872" "2497")
