#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include <assert.h>
#include <stdlib.h>
#include <ansi_fmt.h>
#include <aoc_helpers.h>
#include <aoc_tree.h>
#define TEST_STRING_MAX_LEN (512)
#define TEST_STRING(_p) ((test_string_t *)_p)

typedef struct
{
    struct tree_node_t _node;
    char *_str;
} test_string_t;

static test_string_t *test_string(aoc_tree_node_h _parent, const char *const _str);

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
    aoc_tree_node_h root = tree_blk_init();
    test_string_t *_str1 = test_string(root, "foobar is foobar 1");
    test_string_t *_str2 = test_string(&_str1->_node, "foobar is foobar 2");
    test_string_t *_str3 = test_string(&_str1->_node, "foobar is foobar 3");
    test_string_t *_str4 = test_string(&_str3->_node, "foobar is foobar 4");

    CU_ASSERT(root == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 1", string_node_equal));
    CU_ASSERT(&_str1->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 1", string_node_equal));
    CU_ASSERT(&_str2->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 2", string_node_equal));
    CU_ASSERT(&_str3->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 3", string_node_equal));
    CU_ASSERT(&_str4->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 4", string_node_equal));

    CU_ASSERT(NULL == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 5", string_node_equal));
    CU_ASSERT(4 == aoc_tree_size(&_str1->_node));
    CU_ASSERT(root == _str1->_node._parent);
    CU_ASSERT(NULL == root->_parent);

    aoc_tree_free_node(&_str4->_node, test_string_free);

    CU_ASSERT(3 == aoc_tree_size(&_str1->_node));
    CU_ASSERT(NULL == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 4", string_node_equal));
    CU_ASSERT(&_str1->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 1", string_node_equal));
    CU_ASSERT(&_str2->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 2", string_node_equal));
    CU_ASSERT(&_str3->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 3", string_node_equal));
    
    tree_free_all_from_starting_node(&_str1->_node, test_string_free);
    aoc_ans("%s", "tree_test_suite: all tests passed");
}

static test_string_t *test_string(aoc_tree_node_h _parent, const char *const _str)
{
    test_string_t *_ntree = malloc(sizeof(test_string_t));
    assert(_ntree && "_ntree malloc failed");
    memset(_ntree, 0, sizeof(test_string_t));

    _ntree->_str = malloc(strnlen(_str, MAX_LINE_LEN + 1));
    assert(_ntree->_str && "_ntree->_str malloc failed");

    snprintf(_ntree->_str, MAX_LINE_LEN, "%s", _str);
    aoc_tree_node(_parent, &_ntree->_node, free);

    return _ntree;
}

static void test_string_free(void *arg)
{
    test_string_t *_ntree = (test_string_t *)arg;
    free(_ntree->_str);
    free(arg);
}

static bool string_node_equal(void *_a, void *_b)
{
    return 0 == strncmp(TEST_STRING(_a)->_str, (char *)_b, TEST_STRING_MAX_LEN);
}