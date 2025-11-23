#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context_t
{
    aoc_2d_engine_t _eng;
    aoc_2d_object_t _head;
    struct ll_context_t _tails;
    struct ll_context_t _movs;
    struct ll_context_t _tailPos;
    aoc_2d_object_t _lastMovedLink;
    coord_t _prevPosFromLastMovedLink;
    int result;
};

#define CTX_CAST(_p) ((struct context_t *)_p)

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

typedef struct movement_t
{
    struct ll_node_t _node;
    AOC_2D_DIR dir;
    size_t steps;
} movement_t;

typedef struct coord_tracker_t
{
    struct ll_node_t _node;
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
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    coord_tracker_t _posTrack = {._pos = {._x = _pos->_x, _pos->_y}};
    if (NULL == ll_find_node_by_property(&_ctx->_tailPos, (void *)&_posTrack, coord_compare))
    {
        coord_tracker_t *_npos = malloc(sizeof(coord_tracker_t));
        if (!_npos)
            return ENOMEM;
        _npos->_pos._x = _pos->_x;
        _npos->_pos._y = _pos->_y;
        int ret = ll_node_append(&_ctx->_tailPos, NODE_CAST(_npos));
        if (ret)
            free(_npos);
        return ret;
    }
    return EALREADY;
}

static int prologue(struct solutionCtrlBlock_t *_blk)
{
    int ret = 1;
    _blk->_data = malloc(sizeof(struct context_t));
    if (!_blk->_data)
        return ENOMEM;
    struct context_t *_ctx = CTX_CAST(_blk->_data);

    _ctx->result = 0;

    coord_t resolution = {._x = 1, ._y = 1};
    coord_t spaceing = {._x = 1, ._y = 1};
    _ctx->_eng = engine_create(resolution, spaceing, '.');
    engine_deactivate_drawing(_ctx->_eng);

    _ctx->_head = NULL;

    ll_blk_init(&_ctx->_movs);
    ll_blk_init(&_ctx->_tailPos);
    ll_blk_init(&_ctx->_tails);

    aoc_engine_resize_one_direction(_ctx->_eng, MAP_MID_SIZE, AOC_DIR_RIGHT);
    aoc_engine_resize_one_direction(_ctx->_eng, MAP_MID_SIZE, AOC_DIR_LEFT);
    aoc_engine_resize_one_direction(_ctx->_eng, MAP_MID_SIZE, AOC_DIR_UP);
    aoc_engine_resize_one_direction(_ctx->_eng, MAP_MID_SIZE, AOC_DIR_DOWN);

    coord_t start = {._x = MAP_MID_SIZE, ._y = MAP_MID_SIZE};
    _ctx->_head = eng_obj_create(_ctx->_eng, "head", &start, "H", OBJ_PROPERTY_NO_COLLISION | OBJ_PROPERTY_MOBILE);

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
        aoc_2d_object_t ntail = eng_obj_create(_ctx->_eng, fullname, &start, name, OBJ_PROPERTY_NO_COLLISION | OBJ_PROPERTY_NO_COLLISION);

        if (!ntail)
            ret = ENOMEM;
        if (ret)
            goto error;
        else
            ret = aoc_engine_append_obj(_ctx->_eng, ntail);
        if (ret)
            goto error;
        ll_node_append(&_ctx->_tails, NODE_CAST(ntail));
    }
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

        ll_free_all(&_ctx->_movs, free);
        ll_free_all(&_ctx->_tailPos, free);
        ll_free_all(&_ctx->_tails, eng_obj_free);
    }
    free(_blk->_data);
    return ret;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    char dir = 'R';
    struct context_t *_ctx = CTX_CAST(_blk->_data);
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
            ll_node_append(&_ctx->_movs, NODE_CAST(nmov));
        if (!nmov)
            aoc_err("Error parsing %s", _blk->_str);
    }
    return 0;
}

static void int_refresh_link(struct solutionCtrlBlock_t *_blk, aoc_2d_object_t _head, aoc_2d_object_t _tail, AOC_2D_DIR _dir)
{
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    coord_tracker_t *_npos = NULL;

    if (_ctx->_head == _head)
    {
        if (aoc_engine_step_object_and_redraw(_ctx->_eng, _head, _dir, NULL) == ERANGE)
            aoc_engine_prompt_extra_stats_as_err(_ctx->_eng, "collistion while moving %s", aoc_engine_get_obj_name(_head));
    }
    if (1 < aoc_engine_get_dist_between_objects(_ctx->_eng, _head, _tail))
    {
        if (aoc_engine_move_one_step_towards(_ctx->_eng, _tail, aoc_engine_get_object_position(_ctx->_eng, _head)) == ERANGE)
            aoc_engine_prompt_extra_stats_as_err(_ctx->_eng, "collistion while moving %s", aoc_engine_get_obj_name(_tail));

        coord_t _prevTailPosition = aoc_engine_get_object_position(_ctx->_eng, _tail);
        coord_t _npos = aoc_engine_get_object_position(_ctx->_eng, _tail);
    }
    _ctx->_lastMovedLink = _tail;

    if (0 == strcmp(aoc_engine_get_obj_name(_tail), "tail 9"))
    {
        coord_t _prevTailPosition = aoc_engine_get_object_position(_ctx->_eng, _tail);
        track_tail(_blk, &_prevTailPosition);
    }
    aoc_engine_list_objects(_ctx->_eng);
    engine_cursor_user_stats(_ctx->_eng);
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    movement_t *nmov = NULL;
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    LL_FOREACH(_node, _ctx->_movs)
    {
        nmov = MOV_CAST(_node);
        coord_tracker_t *_npos = NULL;
        for (size_t ii = 0; ii < nmov->steps; ii++)
        {
            _ctx->_lastMovedLink = _ctx->_head;
            LL_FOREACH(_node, _ctx->_tails)
            {
                aoc_2d_object_t _tail = CAST(aoc_2d_object_t, _node);
                int_refresh_link(_blk, _ctx->_lastMovedLink, _tail, nmov->dir);
            } 
        }
    }

    LL_FOREACH(pos_node, _ctx->_tailPos)
    {
        coord_tracker_t *_npos = CAST(coord_tracker_t *, pos_node);
        engine_draw_symbol_at(_ctx->_eng, &_npos->_pos, "#");
    }
    engine_free(_ctx->_eng);
    size_t result = aoc_ll_size(&_ctx->_tailPos);

    ll_free_all(&_ctx->_movs, free);
    ll_free_all(&_ctx->_tailPos, free);
    free(_blk->_data);
    return result;
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue};
struct solutionCtrlBlock_t *part2 = &privPart2;