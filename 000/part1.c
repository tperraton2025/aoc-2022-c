#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context_t
{
    int result;
};

#define CTX_CAST(_p) ((struct context_t *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk)
{
    _blk->_data = malloc(sizeof(struct context_t));
    if (!_blk->_data)
        return ENOMEM;
    memset(_blk->_data, 0, sizeof(struct context_t));
    CTX_CAST(_blk->_data)
        ->result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    int result = _ctx->result;
    aoc_ans("AOC 2022 %s solution is %d", _blk->_name, result);
    return result;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;