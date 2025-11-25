#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include <assert.h>
#include <stdlib.h>
#include <ansi_fmt.h>
#include <aoc_helpers.h>
#include <aoc_ring_buffer.h>
#define TEST_STRING_MAX_LEN (512)

typedef struct
{
    char *name;
} _rb_test_struct_t;

void rb_test_suite();

int rb_suite_create()
{
    CU_pSuite suite = NULL;
    // Add our suite to the registry
    suite = CU_add_suite("AOC ring buffer tests", NULL, NULL);
    if (NULL == suite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_add_test(suite, "ring_buffer_test_suite", rb_test_suite);
    return EXIT_SUCCESS;
}

void rb_test_suite()
{
    aoc_ring_buffer_t _bf = aoc_ring_buffer(sizeof(_rb_test_struct_t), 10);
    CU_ASSERT(NULL != _bf);

    _rb_test_struct_t _str1 = {.name = "foo 1"};
    _rb_test_struct_t _str2 = {.name = "foo 2"};
    _rb_test_struct_t _str3 = {.name = "foo 3"};
    _rb_test_struct_t _str4 = {.name = "foo 4"};

    _rb_test_struct_t *_strings[] = {&_str1, &_str2, &_str3, &_str4};

    for (size_t _ii = 0; _ii < sizeof(_strings) / sizeof(_strings[0]); _ii++)
    {
        CU_ASSERT(0 == aoc_ring_buffer_send_front(_bf, (void *)_strings[_ii]));
    }

    CU_ASSERT(aoc_ring_buffer_available(_bf) == 6);

    for (size_t _ii = 0; _ii < sizeof(_strings) / sizeof(_strings[0]); _ii++)
    {
        _rb_test_struct_t _rx_item = {0};
        CU_ASSERT(0 == aoc_ring_buffer_receive_back(_bf, &_rx_item));
        CU_ASSERT(NULL != _rx_item.name);
        if (_rx_item.name)
            CU_ASSERT_STRING_EQUAL(_rx_item.name, _strings[_ii]->name);
    }

    CU_ASSERT(aoc_ring_buffer_available(_bf) == 10);
    aoc_ring_buffer_free(_bf);

    aoc_ans("%s", "ring_buffer_test_suite: all tests passed");
}
