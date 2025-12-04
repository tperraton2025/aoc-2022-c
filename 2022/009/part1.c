#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <aoc_parser.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context
{
    aoc_2d_engine_h _eng;
    aoc_2d_object_h _head;
    aoc_2d_object_h _tail;

    aoc_2d_object_h _lastMovedLink;
    coord_t _prevPosFromLastMovedLink;
    struct dll_head _movs;
    struct dll_head _tailPos;
    int result;
};

#define CTX_CAST(_p) ((struct context *)_p)

#define MOV_CAST(_p) ((movement_t *)_p)
#define MAP_MID_SIZE (1024)

static AOC_2D_DIR char_to_AOC_DIR(char *_c)
{
    switch (*_c)
    {
    case 'R':
        return AOC_DIR_RIGHT;
    case 'U':
        return AOC_DIR_UP;
    case 'L':
        return AOC_DIR_LEFT;
    case 'D':
        return AOC_DIR_DOWN;
    default:
        return AOC_DIR_MAX;
    }
}

typedef struct
{
    struct dll_node _node;
    AOC_2D_DIR dir;
    size_t steps;
} movement_t;
 
 

static int track_tail(struct solutionCtrlBlock_t *_blk, coord_t *_pos)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    coord_tracker_t *_npos = NULL;
    _npos = malloc(sizeof(coord_tracker_t));
    if (!_npos)
        return ENOMEM;

    _npos->_coord._x = _pos->_x;
    _npos->_coord._y = _pos->_y;
    if (NULL == dll_find_node_by_property(&_ctx->_tailPos, (void *)_npos, coord_compare))
        return dll_node_append(&_ctx->_tailPos, NODE_CAST(_npos));
    else
        FREE(_npos);
    return EALREADY;
}

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR,  _blk->_name);
    int ret = 0;
    TRY_RAII_MALLOC(_blk->_data, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    struct context *_ctx = CTX_CAST(_blk->_data);

    _ctx->result = 0;

    coord_t _prelcoordmaxima = {._x = 20, ._y = 20};

    dll_head_init(&_ctx->_movs);
    dll_head_init(&_ctx->_tailPos);

    bool _drawing = true;
    size_t delay = 0;
    size_t canvassize = MAP_MID_SIZE * 2;
    if (argc >= 2)
        for (int _iarg = 0; _iarg < argc; _iarg++)
        {
            if (0 == strcmp(argv[_iarg], "--nodraw"))
                _drawing = false;
            if ((0 == strcmp(argv[_iarg], "--draw-delay")) && argc > (_iarg + 1))
                sscanf(argv[_iarg + 1], "%3lu", &delay);
            if ((0 == strcmp(argv[_iarg], "--canvas-size")) && argc > (_iarg + 1))
                sscanf(argv[_iarg + 1], "%3lu", &canvassize);
        }

    _ctx->_eng = engine_create(&_prelcoordmaxima, '.', 0);
    if (!_ctx->_eng)
        goto error;

    if (128 < canvassize || !_drawing)
    {
        aoc_info("canvas size is large (%ld), deactivating drawings for readability", canvassize);
        engine_deactivate_drawing(_ctx->_eng);
    }

    eng_set_refresh_delay(_ctx->_eng, delay);

    aoc_engine_extend_one_direction(_ctx->_eng, canvassize, AOC_DIR_RIGHT);
    aoc_engine_extend_one_direction(_ctx->_eng, canvassize, AOC_DIR_DOWN);

    coord_t start = {._x = canvassize >> 1, ._y = canvassize >> 1};
    _ctx->_head = aoc_engine_object(_ctx->_eng, "head", &start, "H", OBJ_PROPERTY_NO_COLLISION | OBJ_PROPERTY_MOBILE);
    _ctx->_tail = aoc_engine_object(_ctx->_eng, "tail", &start, "T", OBJ_PROPERTY_NO_COLLISION | OBJ_PROPERTY_NO_COLLISION);

    if (!_ctx->_head || !_ctx->_tail)
    {
        ret = ENOMEM;
        goto error;
    }

    ret = aoc_engine_append_obj(_ctx->_eng, _ctx->_head);
    if (ret)
        goto error;
    ret = aoc_engine_append_obj(_ctx->_eng, _ctx->_tail);
    if (ret)
        goto error;

    track_tail(_blk, &start);
    engine_draw(_ctx->_eng);
    aoc_engine_prompt_obj_list(_ctx->_eng);
    return 0;

error:
    if (_ctx->_eng)
        engine_free(_ctx->_eng);
    else
    {
        if (_ctx->_head)
            FREE(_ctx->_head);
        if (_ctx->_tail)
            FREE(_ctx->_tail);
    }
    FREE(_blk->_data);
    return ret;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    char dir = 'R';
    struct context *_ctx = CTX_CAST(_blk->_data);
    size_t cnt = 0;
    movement_t *nmov = NULL;
    if (2 == sscanf(_blk->_str, "%c %ld", &dir, &cnt))
    {
        TRY_RAII_MALLOC(nmov, sizeof(movement_t));
        if (!nmov)
            return ENOMEM;
        nmov->steps = cnt;
        nmov->dir = char_to_AOC_DIR(&dir);
        if (nmov->dir >= AOC_DIR_MAX)
            goto error;

        if (dll_node_append(&_ctx->_movs, NODE_CAST(nmov)))
            goto error;
        return 0;
    error:
        aoc_err("Error parsing %s", _blk->_str);
        FREE(nmov);
        return 1;
    }
    return 0;
}

static void int_refresh_link(struct solutionCtrlBlock_t *_blk, aoc_2d_object_h _head, aoc_2d_object_h _tail, AOC_2D_DIR _dir)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    coord_tracker_t *_npos = NULL;

    if (_ctx->_head == _head)
    {
        _ctx->_lastMovedLink = _head;
        _ctx->_prevPosFromLastMovedLink = aoc_engine_get_object_position(_ctx->_eng, _ctx->_lastMovedLink);
        if (aoc_engine_step_object(_ctx->_eng, _head, 1LU, _dir, NULL))
            aoc_engine_prompt_extra_stats_as_err(_ctx->_eng, "collision while moving %s", aoc_engine_get_obj_name(_head));
    }
    if (1 < aoc_engine_get_dist_between_objects(_ctx->_eng, _head, _tail))
    {
        if (aoc_engine_put_object_and_redraw(_ctx->_eng, _tail, _ctx->_prevPosFromLastMovedLink))
            aoc_engine_prompt_extra_stats_as_err(_ctx->_eng, "collision while moving %s", aoc_engine_get_obj_name(_tail));

        coord_t _npos = aoc_engine_get_object_position(_ctx->_eng, _tail);
        _ctx->_prevPosFromLastMovedLink = aoc_engine_get_object_position(_ctx->_eng, _tail);
        track_tail(_blk, &_ctx->_prevPosFromLastMovedLink);
        _ctx->_lastMovedLink = _tail;
    }
    aoc_engine_prompt_obj_list(_ctx->_eng);
    engine_cursor_user_stats(_ctx->_eng);
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    movement_t *nmov = NULL;
    struct context *_ctx = CTX_CAST(_blk->_data);
    LL_FOREACH(_node, _ctx->_movs)
    {
        nmov = MOV_CAST(_node);
        for (size_t ii = 0; ii < nmov->steps; ii++)
        {
            int_refresh_link(_blk, _ctx->_head, _ctx->_tail, nmov->dir);
        }
    }

    LL_FOREACH(pos_node, _ctx->_tailPos)
    {
        coord_tracker_t *_npos = CAST(coord_tracker_t *, pos_node);
        engine_draw_part_at(_ctx->_eng, &_npos->_coord, "#");
    }

    _ctx->result = dll_size(&_ctx->_tailPos);
    aoc_ans("AOC %s %s solution is %d", CONFIG_YEAR, _blk->_name, _ctx->result);
    return 0;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);
    engine_free(_ctx->_eng);
    dll_free_all(&_ctx->_movs, free);
    _ctx->result = dll_size(&_ctx->_tailPos);
    dll_free_all(&_ctx->_tailPos, free);
    FREE(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;