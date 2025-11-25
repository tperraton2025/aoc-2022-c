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
    struct tree_node_t _node;
    char *_str;
} test_string_t;

static test_string_t *test_string(aoc_tree_node_h _parent, const char *const _str);
static int tree_node_integrity(aoc_tree_node_h _parent, aoc_tree_node_h *_exp_children, size_t _child_cnt, aoc_tree_node_h _exp_parent);

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
    aoc_tree_node_h root = aoc_tree_blk_init();
    test_string_t *_str1 = test_string(root, "foobar is foobar 1");

    aoc_tree_node_h _exp_children_for_root[] = {&_str1->_node};
    CU_ASSERT(0 == tree_node_integrity(root, _exp_children_for_root, ARR_DIM(_exp_children_for_root), NULL));

    test_string_t *_str2 = test_string(&_str1->_node, "foobar is foobar 2");
    aoc_tree_node_h _first_exp_children_for_1[] = {&_str2->_node};
    CU_ASSERT(0 == tree_node_integrity(&_str1->_node, _first_exp_children_for_1, ARR_DIM(_first_exp_children_for_1), root));

    test_string_t *_str3 = test_string(&_str1->_node, "foobar is foobar 3");
    test_string_t *_str4 = test_string(&_str3->_node, "foobar is foobar 4");

    aoc_tree_node_h _exp_children_for_1[] = {&_str2->_node, &_str3->_node};
    aoc_tree_node_h _exp_children_for_3[] = {&_str4->_node};

    CU_ASSERT(0 == tree_node_integrity(&_str1->_node, _exp_children_for_1, ARR_DIM(_exp_children_for_1), root));
    CU_ASSERT(0 == tree_node_integrity(&_str3->_node, _exp_children_for_3, ARR_DIM(_exp_children_for_3), (aoc_tree_node_h)&_str1->_node));

    CU_ASSERT(&_str1->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 1", string_node_equal));
    CU_ASSERT(&_str2->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 2", string_node_equal));
    CU_ASSERT(&_str3->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 3", string_node_equal));
    CU_ASSERT(&_str4->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 4", string_node_equal));

    CU_ASSERT(NULL == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 5", string_node_equal));
    CU_ASSERT(4 == aoc_tree_size(&_str1->_node));
    CU_ASSERT(root == _str1->_node._parent);
    CU_ASSERT(NULL == root->_parent);

    aoc_tree_free_node(&_str4->_node);

    CU_ASSERT(0 == tree_node_integrity(&_str1->_node, _exp_children_for_1, ARR_DIM(_exp_children_for_1), root));
    CU_ASSERT(0 == tree_node_integrity(&_str3->_node, NULL, 0, (aoc_tree_node_h)&_str1->_node));

    CU_ASSERT(3 == aoc_tree_size(&_str1->_node));
    CU_ASSERT(NULL == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 4", string_node_equal));
    CU_ASSERT(&_str1->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 1", string_node_equal));
    CU_ASSERT(&_str2->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 2", string_node_equal));
    CU_ASSERT(&_str3->_node == aoc_tree_search_node_by_property(&_str1->_node, (void *)"foobar is foobar 3", string_node_equal));

    tree_free_all_from_starting_node(&_str1->_node);
    aoc_ans("%s", "\ntree_test_suite: all tests passed");
}

static test_string_t *test_string(aoc_tree_node_h _parent, const char *const _str)
{
    test_string_t *_ntree = malloc(sizeof(test_string_t));
    assert(_ntree && "_ntree malloc failed");
    memset(_ntree, 0, sizeof(test_string_t));

    _ntree->_str = malloc(strnlen(_str, MAX_LINE_LEN + 1) + 1);
    assert(_ntree->_str && "_ntree->_str malloc failed");

    snprintf(_ntree->_str, MAX_LINE_LEN, "%s", _str);
    aoc_tree_node(_parent, &_ntree->_node, test_string_free);

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

static int tree_node_integrity(aoc_tree_node_h _node, aoc_tree_node_h *_exp_children, size_t _child_cnt, aoc_tree_node_h _exp_parent)
{
    if (!_node)
        return EINVAL;

    CU_ASSERT(_node->_parent == _exp_parent);

    if (_exp_children)
    {
        for (size_t _ii = 0; _ii < _child_cnt; _ii++)
        {
            if (*_exp_children)
            {
                printf("\nin \n");
                struct tree_node_t *_child_node = (struct tree_node_t *)*_exp_children;
                struct ll_node_t *_found_node = ll_find_node_by_property(&_node->_children, ((test_string_t *)_child_node)->_str, string_node_equal);

                // printf("%s not found in %s children", ((test_string_t *)_node)->_str, ((test_string_t *)_child_node)->_str);

                CU_ASSERT(NULL != _found_node)
                if ((0 == _ii) && (NULL == _child_node->_node._prev))
                {
                    printf("%s not NULL inititated children ll\n", ((test_string_t *)_child_node)->_str);
                    CU_ASSERT(NULL == _child_node->_node._prev);
                }
                if ((_child_cnt - 1 == _ii) && (NULL == _child_node->_node._next))
                {
                    printf("%s not NULL terminated children ll\n", ((test_string_t *)_child_node)->_str);
                    CU_ASSERT(NULL == _child_node->_node._next);
                }
            }

            _exp_children += 1;
        }
    }
    else
    {
        CU_ASSERT(0 == aoc_ll_size(&_node->_children));
        CU_ASSERT(NULL == _node->_children._first);
        CU_ASSERT(NULL == _node->_children._last);
    }
    return 0;
}