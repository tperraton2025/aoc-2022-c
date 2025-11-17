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
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

set_target_properties( ${DAY}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(INFO " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(INFO " ${CMAKE_BINARY_DIR}")
add_definitions(-DCONFIG_DAY="${DAY}")

# unit tests
include(CTest)
add_test(NAME ${DAY}test COMMAND ${DAY} "./data/test1.txt")
add_test(NAME ${DAY}input COMMAND ${DAY} "./data/input.txt")

set(PROLOGUE1 "Welcome to AOC 2022 ${DAY} part 1")
set(PROLOGUE2 "Welcome to AOC 2022 ${DAY} part 2")
set(startRegex1 PROLOGUE1)
set(startRegex2 PROLOGUE2)

set(TEST1REGEX "AOC 2022 ${DAY} part 1 solution is 24000")
set(TEST2REGEX "AOC 2022 ${DAY} part 2 solution is 45000")

set(INPUT1REGEX "AOC 2022 ${DAY} part 1 solution is 72511")
set(INPUT2REGEX "AOC 2022 ${DAY} part 2 solution is 212117")

message(INFO " ${DAY}test requires prompt: ${PROLOGUE1}")
message(INFO " ${DAY}test requires prompt: ${TEST1REGEX}")
message(INFO " ${DAY}test requires prompt: ${PROLOGUE2}")
message(INFO " ${DAY}test requires prompt: ${TEST2REGEX}")

message(INFO " ${DAY}input requires prompt: ${PROLOGUE1}")
message(INFO " ${DAY}input requires prompt: ${INPUT1REGEX}")
message(INFO " ${DAY}input requires prompt: ${PROLOGUE2}")
message(INFO " ${DAY}input requires prompt: ${INPUT2REGEX}")

set_property(TEST ${DAY}test PROPERTY PASS_REGULAR_EXPRESSION "${startRegex1}")
set_property(TEST ${DAY}test PROPERTY PASS_REGULAR_EXPRESSION "${TEST1REGEX}")
set_property(TEST ${DAY}test PROPERTY PASS_REGULAR_EXPRESSION "${startRegex2}")
set_property(TEST ${DAY}test PROPERTY PASS_REGULAR_EXPRESSION "${TEST2REGEX}")


set_property(TEST ${DAY}input PROPERTY PASS_REGULAR_EXPRESSION "${startRegex1}")
set_property(TEST ${DAY}input PROPERTY PASS_REGULAR_EXPRESSION "${INPUT1REGEX}")
set_property(TEST ${DAY}input PROPERTY PASS_REGULAR_EXPRESSION "${startRegex2}")
set_property(TEST ${DAY}input PROPERTY PASS_REGULAR_EXPRESSION "${INPUT2REGEX}")
