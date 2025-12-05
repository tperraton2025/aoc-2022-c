#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context
{
    aoc_2d_engine_h _eng;
    aoc_2d_object_h _head;
    struct dll_head _tails;
    struct dll_head _movs;
    struct dll_head _tailPos;
    aoc_2d_object_h _lastMovedLink;
    coord_t _prevPosFromLastMovedLink;
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
    coord_tracker_t _posTrack = {._coord = {._x = _pos->_x, _pos->_y}};
    if (NULL == dll_find_node_by_property(&_ctx->_tailPos, (void *)&_posTrack, coord_compare))
    {
        coord_tracker_h _npos = malloc(sizeof(coord_tracker_t));
        if (!_npos)
            return ENOMEM;
        _npos->_coord._x = _pos->_x;
        _npos->_coord._y = _pos->_y;
        int ret = dll_node_append(&_ctx->_tailPos, NODE_CAST(_npos));
        if (ret)
            FREE(_npos);
        return ret;
    }
    return EALREADY;
}

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR,  _blk->_name);
    int ret = 0;
    _blk->_data = malloc(sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    struct context *_ctx = CTX_CAST(_blk->_data);

    _ctx->result = 0;

    coord_t _prelcoordmaxima = {._x = 20, ._y = 20};

    _ctx->_head = NULL;

    dll_head_init(&_ctx->_movs);
    dll_head_init(&_ctx->_tailPos);
    dll_head_init(&_ctx->_tails);

    bool _drawing = true;
    size_t delay = 0;
    size_t canvassize = MAP_MID_SIZE * 2;

    if (argc >= 2)
        for (int _iarg = 0; _iarg < argc && !ret; _iarg++)
        {
            if (0 == strcmp(argv[_iarg], "--nodraw"))
                _drawing = false;
            if ((0 == strcmp(argv[_iarg], "--draw-delay")) && argc > (_iarg + 1))
                ret = 1 == sscanf(argv[_iarg + 1], "%3lu", &delay) ? 0 : EINVAL;
            if ((0 == strcmp(argv[_iarg], "--canvas-size")) && argc > (_iarg + 1))
                ret = 1 == sscanf(argv[_iarg + 1], "%3lu", &canvassize) ? 0 : EINVAL;
        }

    if (ret)
        goto error;

    _ctx->_eng = engine_create(&_prelcoordmaxima, '~', 0);

    if (!_ctx->_eng)
    {
        ret = ENOMEM;
        goto error;
    }

    eng_set_refresh_delay(_ctx->_eng, delay);
    aoc_engine_extend_one_direction(_ctx->_eng, canvassize, AOC_DIR_RIGHT);
    aoc_engine_extend_one_direction(_ctx->_eng, canvassize, AOC_DIR_DOWN);

    coord_t start = {._x = canvassize >> 1, ._y = canvassize >> 1};
    _ctx->_head = aoc_engine_object(_ctx->_eng, "head", &start, "H", OBJ_PROPERTY_NO_COLLISION | OBJ_PROPERTY_MOBILE);

    if (!_ctx->_head)
    {
        ret = ENOMEM;
        goto error;
    }

    ret = aoc_engine_append_obj(_ctx->_eng, _ctx->_head);
    if (ret)
        goto error;

    for (size_t ii = 1; ii < 10; ii++)
    {
        char name[] = "1";
        sprintf(name, "%1ld", ii);
        char fullname[] = "tail %1ld";
        sprintf(fullname, "tail %1ld", ii);
        aoc_2d_object_h ntail = aoc_engine_object(_ctx->_eng, fullname, &start, name, OBJ_PROPERTY_NO_COLLISION | OBJ_PROPERTY_NO_COLLISION);

        if (!ntail)
            ret = ENOMEM;
        if (ret)
            goto error;
        else
            ret = aoc_engine_append_obj(_ctx->_eng, ntail);
        if (ret)
            goto error;
        dll_node_append(&_ctx->_tails, NODE_CAST(ntail));
    }
    track_tail(_blk, &start);

    if (128 < canvassize || !_drawing)
    { //! TODO: should be automatic in engine lib
        aoc_info("canvas size is large (%ld), deactivating drawings for readability", canvassize);
        engine_deactivate_drawing(_ctx->_eng);
    }

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

        dll_free_all(&_ctx->_movs, free);
        dll_free_all(&_ctx->_tailPos, free);
        dll_free_all(&_ctx->_tails, aoc_engine_free_object);
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
        nmov = malloc(sizeof(movement_t));
        if (!nmov)
            return ENOMEM;
        nmov->steps = cnt;
        nmov->dir = char_to_AOC_DIR(&dir);
        if (nmov->dir >= AOC_DIR_MAX)
            FREE(nmov);
        if (nmov)
            dll_node_append(&_ctx->_movs, NODE_CAST(nmov));
        if (!nmov)
            aoc_err("Error parsing %s", _blk->_str);
    }
    return 0;
}

static void int_refresh_link(struct solutionCtrlBlock_t *_blk, aoc_2d_object_h _head, aoc_2d_object_h _tail, AOC_2D_DIR _dir)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    coord_tracker_h _npos = NULL;

    if (_ctx->_head == _head)
    {
        if (aoc_engine_step_object(_ctx->_eng, _head, _dir, 1LU, NULL) == ERANGE)
            aoc_engine_prompt_extra_stats_as_err(_ctx->_eng, "collision while moving %s", aoc_engine_get_obj_name(_head));
    }
    if (1 < aoc_engine_get_dist_between_objects(_ctx->_eng, _head, _tail))
    {
        if (aoc_engine_move_one_step_towards(_ctx->_eng, _tail, aoc_engine_get_object_position(_ctx->_eng, _head)) == ERANGE)
            aoc_engine_prompt_extra_stats_as_err(_ctx->_eng, "collision while moving %s", aoc_engine_get_obj_name(_tail));

        coord_t _prevTailPosition = aoc_engine_get_object_position(_ctx->_eng, _tail);
        coord_t _npos = aoc_engine_get_object_position(_ctx->_eng, _tail);
    }
    _ctx->_lastMovedLink = _tail;

    if (0 == strcmp(aoc_engine_get_obj_name(_tail), "tail 9"))
    {
        coord_t _prevTailPosition = aoc_engine_get_object_position(_ctx->_eng, _tail);
        track_tail(_blk, &_prevTailPosition);
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
        coord_tracker_h _npos = NULL;
        for (size_t ii = 0; ii < nmov->steps; ii++)
        {
            _ctx->_lastMovedLink = _ctx->_head;
            LL_FOREACH(_node, _ctx->_tails)
            {
                aoc_2d_object_h _tail = CAST(aoc_2d_object_h, _node);
                int_refresh_link(_blk, _ctx->_lastMovedLink, _tail, nmov->dir);
            }
        }
    }

    LL_FOREACH(pos_node, _ctx->_tailPos)
    {
        coord_tracker_h _npos = CAST(coord_tracker_h , pos_node);
        engine_draw_part_at(_ctx->_eng, &_npos->_coord, "#");
    }
    int result = dll_size(&_ctx->_tailPos);
    aoc_ans("AOC %s %s solution is %d", CONFIG_YEAR, _blk->_name, result);

    return result;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);
    engine_free(_ctx->_eng);
    dll_free_all(&_ctx->_tailPos, free);
    dll_free_all(&_ctx->_movs, free);
    FREE(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part2 = &privPart2;