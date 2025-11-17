cmake_minimum_required(VERSION 3.10.0)
set(FOLDER "")
string(REGEX MATCH "/[0-9][0-9][0-9]$" FOLDER ${CMAKE_SOURCE_DIR})
string(REPLACE "/" "" FOLDER ${FOLDER})

set(DAY "day${FOLDER}")
project(
  ${DAY}
  VERSION 0.1.0
  LANGUAGES C CXX)

add_subdirectory(${CMAKE_SOURCE_DIR}/../lib ${CMAKE_SOURCE_DIR}/../lib
                 EXCLUDE_FROM_ALL)

add_executable(${DAY} main.c part1.c part2.c)
target_include_directories(${DAY} PRIVATE ${CMAKE_SOURCE_DIR}
                                          ${CMAKE_SOURCE_DIR}/../lib/inc)

target_link_libraries(${DAY} libaochelpers)
set(CMAKE_BUILD_TYPE DEBUG)
add_definitions(-DCONFIG_DAY="${DAY}")

# unit tests
include(CTest)
add_test(NAME ${DAY}test COMMAND ${DAY} "./data/test1.txt")
add_test(NAME ${DAY}input COMMAND ${DAY} "./data/input.txt")

set(PROLOGUE1 "Welcome to AOC 2022 ${DAY} part 1")
set(PROLOGUE2 "Welcome to AOC 2022 ${DAY} part 2")

function(add_test_regex test result1 result2)
  set(result1 "AOC 2022 ${DAY} part 1 solution is ${result1}")
  set(result2 "AOC 2022 ${DAY} part 2 solution is ${result2}")
  message(INFO " ${test} requires prompts:")
  message(INFO " ${result1}")
  message(INFO " ${result2}")
  set_property(TEST ${test} PROPERTY PASS_REGULAR_EXPRESSION "${PROLOGUE1}")
  set_property(TEST ${test} PROPERTY PASS_REGULAR_EXPRESSION "${result1}")
  set_property(TEST ${test} PROPERTY PASS_REGULAR_EXPRESSION "${PROLOGUE2}")
  set_property(TEST ${test} PROPERTY PASS_REGULAR_EXPRESSION "${result2}")
endfunction()

add_test_regex(${DAY}test "0" "0")
add_test_regex(${DAY}input "0" "0")

