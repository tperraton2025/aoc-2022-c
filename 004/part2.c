#include <aoc_helpers.h>
#include <aoc_ranges.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
 
struct context_t
{
    struct range_t _a;
    struct range_t _b;
    int result;
};

#define CTX_CAST(_p) ((struct context_t *)_p)

static bool full_overlap(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    return (A_AROUND_B(_ctx->_a, _ctx->_b)) || (A_AROUND_B(_ctx->_b, _ctx->_a));
}

static bool partial_overlap(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    return ((_ctx->_a._max >= _ctx->_b._min) && (_ctx->_a._max <= _ctx->_b._max) || (_ctx->_a._min >= _ctx->_b._min) && (_ctx->_a._min <= _ctx->_b._max));
}

static int prologue(struct solutionCtrlBlock_t *_blk)
{
    _blk->_data = malloc(sizeof(struct context_t));
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    CTX_CAST(_blk->_data)->result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    if (4 == sscanf(_blk->_str, "%ld-%ld,%ld-%ld", &_ctx->_a._min, &_ctx->_a._max, &_ctx->_b._min, &_ctx->_b._max))
        _ctx->result += (full_overlap(_blk) || partial_overlap(_blk)) ? 1 : 0;
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    return _ctx->result;
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue};
struct solutionCtrlBlock_t *part2 = &privPart2;