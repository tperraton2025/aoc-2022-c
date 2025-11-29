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

typedef struct
{
    struct dll_node _node;
    coord_t _pos;
} coord_tracker_t;

static bool coord_compare(void *_a, void *_b)
{
    coord_tracker_t *_aNode = CAST(coord_tracker_t *, _a);
    coord_tracker_t *_bNode = CAST(coord_tracker_t *, _b);
    return (_aNode->_pos._x == _bNode->_pos._x) && (_aNode->_pos._y == _bNode->_pos._y);
}

static int track_tail(struct solutionCtrlBlock_t *_blk, coord_t *_pos)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    coord_tracker_t *_npos = NULL;
    _npos = malloc(sizeof(coord_tracker_t));
    if (!_npos)
        return ENOMEM;

    _npos->_pos._x = _pos->_x;
    _npos->_pos._y = _pos->_y;
    if (NULL == dll_find_node_by_property(&_ctx->_tailPos, (void *)_npos, coord_compare))
        return dll_node_append(&_ctx->_tailPos, NODE_CAST(_npos));
    else
        free(_npos);
    return EALREADY;
}

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    int ret = 1;
    _blk->_data = malloc(sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    struct context *_ctx = CTX_CAST(_blk->_data);

    _ctx->result = 0;

    coord_t resolution = {._x = 1, ._y = 1};
    coord_t spaceing = {._x = 1, ._y = 1};
    _ctx->_eng = engine_create(resolution, spaceing, '.');

    engine_deactivate_drawing(_ctx->_eng);
    _ctx->_head = NULL;
    _ctx->_tail = NULL;

    dll_head_init(&_ctx->_movs);
    dll_head_init(&_ctx->_tailPos);

    aoc_engine_resize_one_direction(_ctx->_eng, MAP_MID_SIZE, AOC_DIR_RIGHT);
    aoc_engine_resize_one_direction(_ctx->_eng, MAP_MID_SIZE, AOC_DIR_LEFT);
    aoc_engine_resize_one_direction(_ctx->_eng, MAP_MID_SIZE, AOC_DIR_UP);
    aoc_engine_resize_one_direction(_ctx->_eng, MAP_MID_SIZE, AOC_DIR_DOWN);

    coord_t start = {._x = MAP_MID_SIZE, ._y = MAP_MID_SIZE};
    _ctx->_head = eng_obj_create(_ctx->_eng, "head", &start, "H", OBJ_PROPERTY_NO_COLLISION | OBJ_PROPERTY_MOBILE);
    _ctx->_tail = eng_obj_create(_ctx->_eng, "tail", &start, "T", OBJ_PROPERTY_NO_COLLISION | OBJ_PROPERTY_NO_COLLISION);

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
    aoc_engine_list_objects(_ctx->_eng);
    return 0;

error:
    if (_ctx->_eng)
        engine_free(_ctx->_eng);
    else
    {
        if (_ctx->_head)
            free(_ctx->_head);
        if (_ctx->_tail)
            free(_ctx->_tail);
    }
    free(_blk->_data);
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
            free(nmov);
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
    coord_tracker_t *_npos = NULL;

    if (_ctx->_head == _head)
    {
        _ctx->_lastMovedLink = _head;
        _ctx->_prevPosFromLastMovedLink = aoc_engine_get_object_position(_ctx->_eng, _ctx->_lastMovedLink);
        if (aoc_engine_step_object_and_redraw(_ctx->_eng, _head, _dir, NULL))
            aoc_engine_prompt_extra_stats_as_err(_ctx->_eng, "collistion while moving %s", aoc_engine_get_obj_name(_head));
    }
    if (1 < aoc_engine_get_dist_between_objects(_ctx->_eng, _head, _tail))
    {
        if (aoc_engine_put_object_and_redraw(_ctx->_eng, _tail, _ctx->_prevPosFromLastMovedLink))
            aoc_engine_prompt_extra_stats_as_err(_ctx->_eng, "collistion while moving %s", aoc_engine_get_obj_name(_tail));

        coord_t _npos = aoc_engine_get_object_position(_ctx->_eng, _tail);
        _ctx->_prevPosFromLastMovedLink = aoc_engine_get_object_position(_ctx->_eng, _tail);
        track_tail(_blk, &_ctx->_prevPosFromLastMovedLink);
        _ctx->_lastMovedLink = _tail;
    }
    aoc_engine_list_objects(_ctx->_eng);
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
        engine_draw_symbol_at(_ctx->_eng, &_npos->_pos, "#");
    }

    _ctx->result = aoc_dll_size(&_ctx->_tailPos);
    aoc_ans("AOC 2022 %s solution is %d", _blk->_name, _ctx->result);
    return 0;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);
    engine_free(_ctx->_eng);
    dll_free_all(&_ctx->_movs, free);
    _ctx->result = aoc_dll_size(&_ctx->_tailPos);
    dll_free_all(&_ctx->_tailPos, free);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;