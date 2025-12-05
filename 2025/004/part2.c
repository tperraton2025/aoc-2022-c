#include "partx.h"

struct context
{
    aoc_2d_engine_h _engine;
    dll_head_h _rolls;
    dll_head_h _freerolls;
    dll_head_t _parsers;
    size_t result;
};

#define CTX_CAST(_p) ((struct context *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR, _blk->_name);
    _blk->_data = malloc(sizeof(struct context));
    memset(_blk->_data, 0, sizeof(struct context));
    struct context *_ctx = CTX_CAST(_blk->_data);

    dll_head_init(&_ctx->_parsers);

    coord_t _max_xy = {1, 1};
    _ctx->_engine = engine_create(&_max_xy, '.', 0);
    engine_deactivate_drawing(_ctx->_engine);
    parser_append(&_ctx->_parsers, &blockparser, _ctx->_engine);
    _ctx->result = 0;

    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    parse_all(&_ctx->_parsers, _blk->_str);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);

    _ctx->_rolls = engine_get_objects_positions(_ctx->_engine);
    _ctx->_freerolls = enumeraterollsatproximity(_ctx->_rolls);

    while (_ctx->_freerolls->_size)
    {
        _ctx->result += _ctx->_freerolls->_size;
        aoc_ans("removed %lu rolls out of %lu", _ctx->_freerolls->_size, _ctx->_rolls->_size);

        markfreerolls(_ctx);

        dll_free_all(_ctx->_rolls, free);
        dll_free_all(_ctx->_freerolls, free);

        dll_head_init(_ctx->_rolls);
        dll_head_init(_ctx->_freerolls);


        _ctx->_rolls = engine_get_objects_positions(_ctx->_engine);
        _ctx->_freerolls = enumeraterollsatproximity(_ctx->_rolls);
    }

    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, _ctx->result);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    engine_free(_ctx->_engine);

    free(_ctx->_rolls);
    free(_ctx->_freerolls);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part2 = &privPart2;

static void markfreerolls(struct context *_ctx)
{
    LL_FOREACH_P(_posn, _ctx->_freerolls)
    {
        coord_tracker_h _trkh = (coord_tracker_h )_posn;
        coord_t *_posh = &_trkh->_coord;
        aoc_2d_object_h _toremove = aoc_engine_get_obj_by_position(_ctx->_engine, _posh);
        engine_remove_object(_ctx->_engine, _toremove);
    }
}