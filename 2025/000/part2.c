#include"partx.h"

typedef struct context
{
    int result;
} context_t;

typedef context_t *context_h;

#define CTX_CAST(_p) ((context_h )_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR, _blk->_name);
    TRY_RAII_MALLOC(_blk->_data, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM; 
    context_h _ctx = CTX_CAST(_blk->_data);
    _ctx->result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    int result = _ctx->result;
    aoc_ans("AOC %s %s solution is %i", CONFIG_YEAR, _blk->_name, result);
    return result;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    (void)_ctx;
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part2 = &privPart2;