#ifndef AOC_RANGES
#define AOC_RANGES

#include <stddef.h>
#include "aoc_linked_list.h"
#include "aoc_helpers.h"
typedef struct range
{
    size_t _max;
    size_t _min;
} range_t;

typedef range_t *range_h;

typedef struct rangenode
{
    struct dll_node _node;
    range_t _range;
} rangenode_t;
typedef rangenode_t *rangenode_h;
rangenode_h rangenode_ctor();

typedef struct rangelist
{
    dll_head_t _head;
} rangelist_t;
typedef rangelist_t *rangelist_h;

#define ARE_CONSECUTIVE(_a, _b) (_a > _b ? _a == _b + 1 : _b == _a + 1)

#define A_AROUND_B_P(_a, _b) ((_a->_max >= _b->_max) && (_a->_min <= _b->_min))
#define A_AROUND_B(_a, _b) ((_a._max >= _b._max) && (_a._min <= _b._min))
#define N_BETWEEN_AB(_n, _a, _b) (((_n) >= (_a)) && ((_n) <= (_b)))

#define N_IN_RANGE(_n, _ra) (((_n) >= (_ra._min)) && ((_n) <= (_ra._max)))
#define N_IN_RANGE_P(_n, _ra) (((_n) >= (_ra->_min)) && ((_n) <= (_ra->_max)))

#define HIGHEST(_a, _b) ((_a) > (_b) ? (_a) : (_b))
#define LOWEST(_a, _b) ((_a) > (_b) ? (_b) : (_a))

rangelist_h rangelistinit();
int tryinsertrange(rangelist_h list, rangenode_h rangenode);

char *str_rangenode(rangenode_h rangen);

#endif