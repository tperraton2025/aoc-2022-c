
void test_example(void)
{
    CU_ASSERT(1 == 1);                   // test passed if true
    CU_ASSERT_TRUE(1);                   // test passed if true
    CU_ASSERT_FALSE(0);                  // test passed if false
    CU_ASSERT_PTR_NULL(NULL);            // test passed if null
    CU_ASSERT_PTR_NOT_NULL(NULL);        // test pass if not null
    CU_ASSERT_EQUAL(1, 1);               // test passed if equal
    CU_ASSERT_NOT_EQUAL(5, 0)            // test passed if different
    CU_ASSERT_STRING_EQUAL("x", "y")     // test passed if equal
    CU_ASSERT_STRING_NOT_EQUAL("x", "y") // test passed if different
    CU_PASS("msg");                      // mark the test as successful
    CU_FAIL("msg");                      // mark the test as failed
    // see also: CU_ASSERT_PTR_EQUAL, *_FATAL, etc.
}
