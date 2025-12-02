#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context
{
    int _encoder;
    int result;
};

#define CTX_CAST(_p) ((struct context *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR,  _blk->_name);
    _blk->_data = malloc(sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    memset(_blk->_data, 0, sizeof(struct context));
    struct context *_ctx = CTX_CAST(_blk->_data);
    _ctx->_encoder = 50;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    int _prevEncoder = _ctx->_encoder;
    int _increments = 0;
    char _dir;
    if (2 == sscanf(_blk->_str, "%c%i", &_dir, &_increments))
        while (_increments--)
        {
            _ctx->_encoder = (_ctx->_encoder + (_dir == 'R' ? 1 : -1)) % 100;
            if (!_ctx->_encoder)
                _ctx->result++;
            if (0 > _ctx->_encoder)
                _ctx->_encoder = 100 + _ctx->_encoder;
        } 
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    int result = _ctx->result;
    aoc_ans("AOC %s %s solution is %i", CONFIG_YEAR, _blk->_name, result);
    return result;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    /* free allocated _blk->_data members first*/
    /* solution specific section */
    /* free _blk->_data itself after wards */
    if (_blk->_data)
        free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part2 = &privPart2;