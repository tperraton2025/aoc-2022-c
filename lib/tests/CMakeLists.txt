cmake_minimum_required(VERSION 3.10.0)
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
find_package(CUnit REQUIRED)

project(
  ${ProjectId}
  VERSION 0.1.0
  LANGUAGES C CXX)

add_executable(${ProjectId} test_suite.c test_linked_lists.c)
set_property(TARGET ${ProjectId} PROPERTY C_STANDARD 11)
target_link_libraries(${ProjectId} ${CUNIT_LIBRARIES} libaochelpers)

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

set_target_properties(
  ${ProjectId}
  PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_test(NAME ${ProjectId}
         COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bin/${ProjectId}" --exe
                 "${ProjectId}test")

add_test_regex(${ProjectId} "ll_test_suite: all tests passed")