#include "partx.h"

typedef struct context
{
    rangelist_h _ranges;
    dll_head_t _parsers;
    dll_head_t _items;

    size_t _result;
} context_t;

typedef context_t *context_h;

#define CTX_CAST(_p) ((context_h)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR, _blk->_name);
    TRY_RAII_MALLOC(_blk->_data, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    context_h _ctx = CTX_CAST(_blk->_data);
    _ctx->_result = 0;
    _ctx->_ranges = rangelistinit();
    dll_head_init(&_ctx->_items);
    dll_head_init(&_ctx->_parsers);

    parser_append(&_ctx->_parsers, &_rangeparser, _ctx->_ranges);
    parser_append(&_ctx->_parsers, &_itemparser, &_ctx->_items);
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    parse_all(&_ctx->_parsers, _blk->_str);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);

    LL_FOREACH(_itemn, _ctx->_items)
    {
        intlistitem_h _itemh = (intlistitem_h)_itemn;
        if (checkexpired(_ctx->_ranges, _itemh))
            _ctx->_result++;
    }

    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, _ctx->_result);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    (void)_ctx; 
    dll_free_all(&_ctx->_items, free);
    dll_free_all(&_ctx->_ranges->_head, free);
    free(_ctx->_ranges);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;

DLL_NODE_CTOR(intlistitem_t, intlistnode_ctor);