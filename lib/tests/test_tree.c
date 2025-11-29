#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include <assert.h>
#include <stdlib.h>
#include <ansi_fmt.h>
#include <aoc_helpers.h>
#include <aoc_tree.h>
#define TEST_STRING_MAX_LEN (512)
#define TEST_STRING(_p) ((test_string_t *)_p)
#define ARR_DIM(_a) (sizeof(_a) / sizeof(_a[0]))
typedef struct
{
    struct tree_node _node;
    char *_str;
} test_string_t;

typedef struct
{
    test_string_t **_arr;
    size_t _len;
} test_str_arr_t;

#define EXPECT_CHILDREN(_tstr, ...)                                              \
    {                                                                            \
        test_string_t *_expected[] = {__VA_ARGS__};                              \
        test_str_arr_t _tsArr = {._arr = _expected, ._len = ARR_DIM(_expected)}; \
        CU_ASSERT(0 == tree_node_integrity(_tstr, &_tsArr));                     \
    }

static test_string_t *test_string(aoc_tree_node_h _parent, const char *const _str);
static int tree_node_integrity(test_string_t *_node, test_str_arr_t *_expct);

static void test_string_free(void *arg);
static void tree_test_suite();
static bool string_node_equal(void *_a, void *_b);

int tree_suite_create()
{
    CU_pSuite suite = NULL;
    // Add our suite to the registry

    suite = CU_add_suite("AOC Linked list tests", NULL, NULL);
    if (NULL == suite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_add_test(suite, "linked_list_test_suite", tree_test_suite);
    return EXIT_SUCCESS;
}

void tree_test_suite()
{
    test_string_t *root = test_string(NULL, "foobar is foobar root");
    test_string_t *_str1 = test_string(&root->_node, "foobar is foobar 1");
    CU_ASSERT(1 == aoc_tree_size(&root->_node));

    EXPECT_CHILDREN(root, _str1);

    test_string_t *_str2 = test_string(&_str1->_node, "foobar is foobar 2");
    EXPECT_CHILDREN(_str1, _str2);

    test_string_t *_str3 = test_string(&_str1->_node, "foobar is foobar 3");
    test_string_t *_str4 = test_string(&_str3->_node, "foobar is foobar 4");
    EXPECT_CHILDREN(_str1, _str2, _str3);
    EXPECT_CHILDREN(_str3, _str4);

    CU_ASSERT(NULL == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 5", string_node_equal));
    CU_ASSERT(&root->_node == _str1->_node._parent);
    CU_ASSERT(NULL == root->_node._parent);

    aoc_tree_free_node(&_str4->_node);

    EXPECT_CHILDREN(root, _str1);

    CU_ASSERT(NULL == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 4", string_node_equal));

    aoc_tree_free(&root->_node);
    aoc_ans("%s", "\ntree_test_suite: all tests passed");
}

static test_string_t *test_string(aoc_tree_node_h _parent, const char *const _str)
{
    test_string_t *_ntstr = malloc(sizeof(test_string_t));
    assert(_ntstr && "_ntstr malloc failed");
    memset(_ntstr, 0, sizeof(test_string_t));

    _ntstr->_str = malloc(strnlen(_str, MAX_LINE_LEN + 1) + 1);
    assert(_ntstr->_str && "_ntstr->_str malloc failed");

    snprintf(_ntstr->_str, MAX_LINE_LEN, "%s", _str);
    aoc_tree_node(_parent, &_ntstr->_node, test_string_free);

    return _ntstr;
}

static void test_string_free(void *arg)
{
    test_string_t *_ntree = (test_string_t *)arg;
    free(_ntree->_str);
}

static bool string_node_equal(void *_a, void *_b)
{
    return 0 == strncmp(TEST_STRING(_a)->_str, (char *)_b, TEST_STRING_MAX_LEN);
}

static int tree_node_integrity(test_string_t *_string, test_str_arr_t *_expct)
{
    if (!_string)
        return EINVAL;

    if (_expct)
    {
        for (size_t _ii = 0; _ii < _expct->_len; _ii++)
        {
            if (_expct->_len)
            {
                test_string_t *_child_tstr = _expct->_arr[_ii];
                struct dll_node *_found_node = dll_find_node_by_property(&_string->_node._dllchildren, _child_tstr->_str, string_node_equal);

                CU_ASSERT(NULL != _found_node)
                if (0 == _ii)
                {
                    CU_ASSERT(NULL == _child_tstr->_node._llnode._prev);
                }

                if (_expct->_len - 1 == _ii)
                    CU_ASSERT(NULL == _child_tstr->_node._llnode._next);

                CU_ASSERT(_child_tstr->_node._parent == &_string->_node);
            }
        }
    }
    else
    {
        CU_ASSERT(0 == aoc_dll_size(&_string->_node._dllchildren));
        CU_ASSERT(NULL == _string->_node._dllchildren._first);
        CU_ASSERT(NULL == _string->_node._dllchildren._last);
    }
    for (size_t _ii = 0; _ii < _expct->_len; _ii++)
    {
        test_string_t *_ctstr = _expct->_arr[_ii];
        char *_expName = _ctstr->_str;
        struct tree_node *_expNode = &_ctstr->_node;
        CU_ASSERT(_expNode == aoc_tree_search_node_by_property(_expNode, (void *)_expName, string_node_equal));
    }
    return 0;
}