#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include <assert.h>
#include <stdlib.h>
#include <ansi_fmt.h>
#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#define TEST_STRING_MAX_LEN (512)
#define TS_CAST(_p) ((_string_test_t *)_p)

typedef struct
{
    struct dll_node _node;
    char *_str;
} _string_test_t;

static _string_test_t *_string_ll_test(const char *const _str);
static void free_test_string(void *_node);
static void ll_test_suite();
static bool _string_node_equal(void *_a, void *_b);

int ll_suite_create()
{
    CU_pSuite suite = NULL;
    // Add our suite to the registry
    suite = CU_add_suite("AOC Linked list tests", NULL, NULL);
    if (NULL == suite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_add_test(suite, "linked_list_test_suite", ll_test_suite);
    return EXIT_SUCCESS;
}

void ll_test_suite()
{
    struct dll_head _ll = {0};
    dll_head_init(&_ll);

    _string_test_t *_str1 = _string_ll_test("foo 1");
    _string_test_t *_str2 = _string_ll_test("foo 2");
    _string_test_t *_str3 = _string_ll_test("foo 3");
    _string_test_t *_str4 = _string_ll_test("foo 4");

    _string_test_t *_strings[] = {_str1,
                                  _str2,
                                  _str3,
                                  _str4};

    for (size_t _ii = 0; _ii < sizeof(_strings) / sizeof(_strings[0]); _ii++)
    {
        dll_node_append(&_ll, (struct dll_node *)_strings[_ii]);
    }

    CU_ASSERT(dll_size(&_ll) == 4);
    CU_ASSERT(TS_CAST(_ll._first->_prev) == NULL);

    CU_ASSERT(TS_CAST(_ll._last) == _str4);
    CU_ASSERT(TS_CAST(_ll._first) == _str1);
    CU_ASSERT(TS_CAST(_ll._first->_next) == _str2);
    CU_ASSERT(TS_CAST(_ll._first->_next->_next) == _str3);
    CU_ASSERT(TS_CAST(_ll._first->_next->_next->_next) == _str4);

    CU_ASSERT(TS_CAST(_ll._first->_next->_next->_next->_next) == NULL);

    dll_node_disconnect(&_ll, _ll._first->_next);

    CU_ASSERT(dll_size(&_ll) == 3);
    CU_ASSERT(TS_CAST(_ll._first->_prev) == NULL);

    CU_ASSERT(TS_CAST(_ll._last) == _str4);
    CU_ASSERT(TS_CAST(_ll._first) == _str1);
    CU_ASSERT(TS_CAST(_ll._first->_next) == _str3);
    CU_ASSERT(TS_CAST(_ll._first->_next->_next) == _str4);
    CU_ASSERT(ENOENT == dll_find_node(&_ll, &_str2->_node))

    CU_ASSERT(TS_CAST(_ll._first->_next->_next->_next) == NULL);

    dll_node_permut(&_ll, _ll._first, _ll._last);

    CU_ASSERT(dll_size(&_ll) == 3);
    CU_ASSERT(TS_CAST(_ll._first->_prev) == NULL);

    CU_ASSERT(TS_CAST(_ll._last) == _str1);
    CU_ASSERT(TS_CAST(_ll._first) == _str4);
    CU_ASSERT(TS_CAST(_ll._first->_next) == _str3);
    CU_ASSERT(TS_CAST(_ll._first->_next->_next) == _str1);

    CU_ASSERT(TS_CAST(_ll._first->_next->_next->_next) == NULL);
    CU_ASSERT(_ll._first->_next->_next == _ll._last);

    dll_node_insert(&_ll, &_str2->_node, _ll._last);

    CU_ASSERT(dll_size(&_ll) == 4);
    CU_ASSERT(TS_CAST(_ll._first->_prev) == NULL);

    CU_ASSERT(TS_CAST(_ll._first) == _str4);
    CU_ASSERT(TS_CAST(_ll._first->_next) == _str3);
    CU_ASSERT(TS_CAST(_ll._first->_next->_next) == _str2);
    CU_ASSERT(TS_CAST(_ll._first->_next->_next->_next) == _str1);

    CU_ASSERT(TS_CAST(_ll._first->_next->_next->_next->_next) == NULL);
    CU_ASSERT(_ll._first->_next->_next->_next == _ll._last);

    CU_ASSERT(1 == dll_count_nodes_by_property(&_ll, _str4->_str, _string_node_equal));
    CU_ASSERT(_str4 == TS_CAST(dll_find_node_by_property(&_ll, _str4->_str, _string_node_equal)));

    dll_free_all(&_ll, free_test_string);
    aoc_ans("%s", "ll_test_suite: all tests passed");
}

static _string_test_t *_string_ll_test(const char *const _str)
{
    _string_test_t *_nstr = malloc(sizeof(_string_test_t));
    assert(_nstr && "_string_ll_test malloc failed");

    _nstr->_str = malloc(strnlen(_str, TEST_STRING_MAX_LEN + 1) + 1);
    assert(_nstr->_str && "_nstr->_str malloc failed");
    sprintf(_nstr->_str, "%s", _str);

    return _nstr;
}

static void free_test_string(void *_node)
{
    _string_test_t *_nstr = (_string_test_t *)_node;
    FREE_AND_CLEAR_P(_nstr->_str);
    _nstr->_str = NULL;
    FREE_AND_CLEAR_P(_node);
    _node = NULL;
}

static bool _string_node_equal(void *_a, void *_b)
{
    return 0 == strncmp((const char *)TS_CAST(_a)->_str, (const char *)_b, TEST_STRING_MAX_LEN);
}