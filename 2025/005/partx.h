#include <aoc_helpers.h>
#include <aoc_ranges.h>
#include <aoc_linked_list.h>
#include <aoc_parser.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct intlistitem
{
    struct dll_node _node;
    size_t _int;
} intlistitem_t;
typedef intlistitem_t *intlistitem_h;

intlistitem_h intlistnode_ctor();

static int parserange(void *arg, char *str)
{
    rangelist_h _rangelist = (rangelist_h)arg;
    size_t _start = 0;
    size_t _end = 0;
    if (2 == sscanf(str, "%lu-%lu", &_start, &_end))
    {
        rangenode_t nrange = {._range = {._max = _end, ._min = _start}};
        tryinsertrange(_rangelist, &nrange);
        return 0;
    }
    return EINVAL;
}

static int parseitem(void *arg, char *str)
{
    dll_head_h _itlist = (dll_head_h)arg;
    size_t _item = 0;
    if (1 == sscanf(str, "%lu\n", &_item))
    {
        intlistitem_h nintlist = intlistnode_ctor();
        nintlist->_int = _item;
        dll_node_append(_itlist, &nintlist->_node);
    }
    return 0;
}

static parser_t _rangeparser = {._name = "ranges", ._func = parserange};
static parser_t _itemparser = {._name = "items", ._func = parseitem};

static bool checkexpired(rangelist_h list, intlistitem_h item)
{
    LL_FOREACH(_rangen, list->_head)
    {
        rangenode_h _rangeh = (rangenode_h)_rangen;
        if (_rangen->_obsolete)
        {
#ifdef AOC_RANGE_VERBOSE_MODE
            aoc_err("%s is obsolete, skipped of %lu", str_rangenode(_rangeh), item->_int);
#endif
            continue;
        }
        if N_IN_RANGE (item->_int, _rangeh->_range)
        {
#ifdef AOC_RANGE_VERBOSE_MODE
            aoc_ans("%s accepts %lu", str_rangenode(_rangeh), item->_int);
#endif
            return true;
        }
#ifdef AOC_RANGE_VERBOSE_MODE
        aoc_warn("%s rejects %lu", str_rangenode(_rangeh), item->_int);
#endif
    }
    return false;
}

static size_t sumrangespans(rangenode_h rangen)
{
    if (rangen->_node._obsolete)
    {
#ifdef AOC_RANGE_VERBOSE_MODE
        aoc_err("%s is obsolete", str_rangenode(rangen));
#endif
        return 0;
    }
    size_t span = (rangen->_range._max - rangen->_range._min) + 1;
#ifdef AOC_RANGE_VERBOSE_MODE
    aoc_ans("%s span is %lu", str_rangenode(rangen), span);
#endif
    return span;
}