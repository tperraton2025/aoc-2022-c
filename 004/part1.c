#include <aoc_helpers.h>
#include <aoc_ranges.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
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

static int prologue(struct solutionCtrlBlock_t *_blk)
{
    _blk->_data = malloc(sizeof(struct context_t));
    if (!_blk->_data)
        return ENOMEM;
    memset(_blk->_data, 0, sizeof(struct context_t));
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    CTX_CAST(_blk->_data)->result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    if (4 == sscanf(_blk->_str, "%ld-%ld,%ld-%ld", &_ctx->_a._min, &_ctx->_a._max, &_ctx->_b._min, &_ctx->_b._max))
        _ctx->result += full_overlap(_blk) ? 1 : 0;
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    int result = CTX_CAST(_blk->_data)->result;
    aoc_ans("AOC 2022 %s solution is %d", _blk->_name, result);
    return result;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CAST(struct context_t *, _blk->_data);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;