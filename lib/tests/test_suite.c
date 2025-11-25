#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include <assert.h>
#include <stdlib.h>
#include <aoc_linked_list.h>

extern int ll_suite_create();
extern int rb_suite_create();
extern int tree_suite_create();

typedef int (*testSuite_t)(CU_pSuite arg);

testSuite_t listSuite[] = {
    ll_suite_create, rb_suite_create, tree_suite_create};

int main()
{
    CU_pSuite pSuite = NULL;
    int res;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    for (int _ii = 0; _ii < sizeof(listSuite) / sizeof(testSuite_t); _ii++)
    {
        res = listSuite[_ii](pSuite);
        if (res == EXIT_FAILURE)
        {
            CU_cleanup_registry();
            return CU_get_error();
        }
    }

    /* Run all tests using the ??? interface */
    /* ... */

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    /* Clean up registry and return */
    CU_cleanup_registry();
    return CU_get_error();
}