function(add_aoc_day_test_regex test result1 result2)
  set(prol1 "Welcome to AOC 2022 ${ProjectId} part 1\n")
  set(result1 ".*AOC 2022 ${ProjectId} part 1 solution is ${result1}.*\n")
  set(prol2 "Welcome to AOC 2022 ${ProjectId} part 2\n")
  set(result2 ".*AOC 2022 ${ProjectId} part 2 solution is ${result2}.*\n")
  set_property(TEST ${test} PROPERTY PASS_REGULAR_EXPRESSION
                                     "${prol1}${result1}${prol2}${result2}")
endfunction()

function(add_test_regex test result)
  set(TEST_REGEX "${TEST_REGEX}.*[\n]+.*")
  set_property(TEST ${test} PROPERTY PASS_REGULAR_EXPRESSION "${TEST_REGEX}")
endfunction()
