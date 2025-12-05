#include "partx.h"

struct context
{
    dll_head_t _originjoltages;
    int result;
};

#define CTX_CAST(_p) ((struct context *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR, _blk->_name);
    _blk->_data = malloc(sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    memset(_blk->_data, 0, sizeof(struct context));
    struct context *_ctx = CTX_CAST(_blk->_data);
    dll_head_init(&_ctx->_originjoltages);
    _ctx->result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    populatejoltageratings(&_ctx->_originjoltages, _blk->_str);

    size_t _newjoltage = 0;

    size_t _decimals = 0;
    size_t _units = 0;

    LL_FOREACH(_joltriter, _ctx->_originjoltages)
    {
        joltage_h _jolt_i = ((joltage_h)_joltriter);

        /* skipping numbers that came too late to give a 2 decimals number*/
        if (_jolt_i->_order > _ctx->_originjoltages._size - 1)
            continue;

        joltage_h _jolt_j = NULL;

        dll_head_h _sortednextjolts = dll_clone_trimmed(&_ctx->_originjoltages, sizeof(joltage_t), (void *)&_jolt_i->_order, isnotajoltagethatcameafter);
        if (!_sortednextjolts)
            continue;

        _jolt_j = (joltage_h)_sortednextjolts->_first;

        if (!_jolt_j)
        {
            dll_free_all(_sortednextjolts, free);
            free(_sortednextjolts);
            continue;
        }

        _decimals = _jolt_i->_rating;
        _units = _jolt_j->_rating;
        _newjoltage = 10 * _decimals + _units;
        dll_free_all(_sortednextjolts, free);
        free(_sortednextjolts);

        joltage_h _jolt_ip1 = (joltage_h)_jolt_i->_node._next;
        if (_jolt_ip1)
            if (_jolt_ip1->_rating != _jolt_i->_rating)
                break;
    }

    _ctx->result += _newjoltage;
    dll_free_all(&_ctx->_originjoltages, free);
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
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;

DLL_NODE_CTOR(joltage_t, joltage_ctor);