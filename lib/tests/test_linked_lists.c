#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdlib.h>

#include <assert.h>
#include <stdlib.h>
#include <ansi_fmt.h>
#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#define TEST_STRING_MAX_LEN (512)
#define TS_CAST(_p) ((teststr_t *)_p)

typedef struct
{
    struct dll_node _node;
    char *_str;
} teststr_t;

typedef teststr_t *teststr_h;

#define ASSERT_LL_INTEGRITY(_ll, ...)                                       \
    {                                                                       \
        teststr_h _exp[] = {__VA_ARGS__};                                   \
        CU_ASSERT(dll_size(&_ll) == ARRAY_DIM(_exp));                       \
        CU_ASSERT(TS_CAST(_ll._first->_prev) == NULL);                      \
        CU_ASSERT(TS_CAST(_ll._first) == _exp[0]);                          \
        CU_ASSERT(TS_CAST(_ll._last) == _exp[ARRAY_DIM(_exp) - 1]);         \
        dll_node_h _node = _ll._first;                                \
        ARR_FOREACH(_ii, _exp)                                              \
        {                                                                   \
            CU_ASSERT(TS_CAST(_node) == _exp[_ii]);                         \
            _node = _node->_next;                                           \
        }                                                                   \
        CU_ASSERT(TS_CAST(_exp[ARRAY_DIM(_exp) - 1])->_node._next == NULL); \
    }

static teststr_h _string_ll_test(const char *const _str);
static dll_node_h _strinsortbyname(dll_node_h arga, dll_node_h argb);
static void print(void *arga, void *argb);
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

    CU_ASSERT_EQUAL(atoi(LL_MAX_LEN_STR), LL_MAX_LEN_LUI);

    teststr_h _str1 = _string_ll_test("foo 1");
    teststr_h _str2 = _string_ll_test("foo 2");
    teststr_h _str3 = _string_ll_test("foo 3");
    teststr_h _str4 = _string_ll_test("foo 4");

    teststr_h _strings[] = {_str1,
                            _str2,
                            _str3,
                            _str4};

    for (size_t _ii = 0; _ii < sizeof(_strings) / sizeof(_strings[0]); _ii++)
    {
        dll_node_append(&_ll, (dll_node_h )_strings[_ii]);
    }
    ASSERT_LL_INTEGRITY(_ll, _str1, _str2, _str3, _str4);

    dll_node_disconnect(&_ll, _ll._first->_next);
    ASSERT_LL_INTEGRITY(_ll, _str1, _str3, _str4);
    CU_ASSERT(ENOENT == dll_find_node(&_ll, &_str2->_node))

    dll_node_swap(&_ll, _ll._first, _ll._last);
    ASSERT_LL_INTEGRITY(_ll, _str4, _str3, _str1);

    dll_node_swap(&_ll, &_str4->_node, &_str3->_node);
    ASSERT_LL_INTEGRITY(_ll, _str3, _str4, _str1);

    dll_node_insert(&_ll, &_str2->_node, _ll._last);

    ASSERT_LL_INTEGRITY(_ll, _str3, _str4, _str2, _str1);

    dll_sort(&_ll, _strinsortbyname);
    ASSERT_LL_INTEGRITY(_ll, _str1, _str2, _str3, _str4);

    CU_ASSERT(1 == dll_count_nodes_by_property(&_ll, _str4->_str, _string_node_equal));
    CU_ASSERT(_str4 == TS_CAST(dll_find_node_by_property(&_ll, _str4->_str, _string_node_equal)));

    dll_free_all(&_ll, free_test_string);
    aoc_info("%s", "ll_test_suite: all tests were ran");
}

static teststr_h _string_ll_test(const char *const _str)
{
    teststr_h _nstr = malloc(sizeof(teststr_t));
    assert(_nstr && "_string_ll_test malloc failed");

    _nstr->_str = malloc(strnlen(_str, TEST_STRING_MAX_LEN + 1) + 1);
    assert(_nstr->_str && "_nstr->_str malloc failed");
    sprintf(_nstr->_str, "%s", _str);

    return _nstr;
}

static void print(void *arga, void *argb)
{
    teststr_h _nstr = (teststr_h)arga;
    printf("%s", _nstr->_str);
}

static void free_test_string(void *_node)
{
    teststr_h _nstr = (teststr_h)_node;
    FREE(_nstr->_str);
    _nstr->_str = NULL;
    FREE(_node);
    _node = NULL;
}

static bool _string_node_equal(void *_a, void *_b)
{
    return 0 == strncmp((const char *)TS_CAST(_a)->_str, (const char *)_b, TEST_STRING_MAX_LEN);
}

static dll_node_h _strinsortbyname(dll_node_h arga, dll_node_h argb)
{
    teststr_h _jolta = (teststr_h)arga;
    teststr_h _joltb = (teststr_h)argb;
    return strcmp(_jolta->_str, _joltb->_str) >= 0 ? arga : argb;
}