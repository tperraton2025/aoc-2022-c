#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <aoc_ranges.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct elfCal
{
    struct dll_node _node;
    char *_name;
    int _count;
    int _calories;
} elfCal_t;

typedef struct context
{
    dll_head_t _head;
    int _maxCalories;
} context_t;

DLL_NODE_CTOR(elfCal_t, elfnode_ctor);

static elfCal_t *elfCtor(dll_head_h head)
{
    elfCal_t *_ret = elfnode_ctor();
    if (!_ret)
        return NULL;

    _ret->_count = dll_size(head);
    _ret->_calories = 0;
    _ret->_name = malloc(sizeof("elfo 999"));
    
    if (!_ret->_name)
    {
        FREE(_ret);
        return NULL;
    }
    sprintf(_ret->_name, "elfo%3d", _ret->_count);

    return _ret;
}

static void freeElf(void *arg)
{
    elfCal_t *_elf = CAST(elfCal_t *, arg);
    if (_elf->_name)
        FREE(_elf->_name);
    if (_elf)
        FREE(_elf);
}

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR,  _blk->_name);
    _blk->_data = malloc(sizeof(context_t));
    context_t *_ctx = CAST(context_t *, _blk->_data);
    if (!_ctx)
        return ENOMEM;
    memset(_ctx, 0, sizeof(context_t));

    dll_head_init(&_ctx->_head);
    elfCal_t *nelf = elfCtor(&_ctx->_head);
    dll_node_append(&_ctx->_head, &nelf->_node);
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    context_t *_ctx = CAST(context_t *, _blk->_data);
    elfCal_t *_lastelf = CAST(elfCal_t *, _ctx->_head._last);

    int _cal = 0;
    if (sscanf(_blk->_str, "%d\n", &_cal))
        if (_cal)
        {
            _lastelf->_calories += _cal;
            _ctx->_maxCalories = HIGHEST(_lastelf->_calories, _ctx->_maxCalories);
        }
        else
        {
            elfCal_t *nelf = elfCtor(&_ctx->_head);
            dll_node_append(&_ctx->_head, &nelf->_node);
        }
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    context_t *_data = CAST(context_t *, _blk->_data);
    int result = _data->_maxCalories;
    aoc_ans("AOC %s %s solution is %d", CONFIG_YEAR, _blk->_name, result);
    return result;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    context_t *_ctx = CAST(context_t *, _blk->_data);

    dll_free_all(&_ctx->_head, freeElf);
    FREE(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;