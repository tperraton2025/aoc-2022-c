#include "part2.h"

static int prologue(struct solutionCtrlBlock_t *_blk)
{
    int ret = 0;

    _blk->_data = malloc(sizeof(struct context_t));
    if (!_blk->_data)
        ret = ENOMEM;

    if (ret)
        goto error;

    struct context_t *_ctx = CTX_CAST(_blk->_data);

    _ctx->result = 0;

    _ctx->_pos._x = 1;
    _ctx->_pos._y = 1;

    ll_blk_init(&_ctx->_cmds);
    ll_blk_init(&_ctx->_BoxStack);
    ll_blk_init(&_ctx->_grippedBoxes);

    coord_t _reso = {3, 1};
    coord_t _spce = {1, 0};
    _ctx->_eng = engine_create(_reso, _spce, '~');
    engine_deactivate_drawing(_ctx->_eng);
    if (_ctx->_eng)
        goto success;

    _block._arg = _blk;
    _cmd._arg = _blk;

    ret = ENOMEM;
    goto cleanup;

success:
    return 0;

cleanup:
    free(_blk->_data);
error:
    return ret;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    parse_all(_ctx, _blk->_str);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    char test[16] = {0};

    coord_t _boundaries = aoc_engine_get_boundaries(_ctx->_eng);

    aoc_engine_resize_one_direction(_ctx->_eng, 50, AOC_DIR_DOWN);
    engine_draw(_ctx->_eng);
    crane_action(_ctx);
    aoc_spell_ans(_ctx);

    aoc_ans("AOC 2022 %s solution is %s", _blk->_name, _ctx->spelling);

    int ret = _ctx->result;
    return ret;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CAST(struct context_t *, _blk->_data);
    engine_free(_ctx->_eng);
    free(_ctx->spelling);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part2 = &privPart2;

static int crate_lift(struct context_t *_ctx, command_t *_cmd)
{
    coord_t boundaries = aoc_engine_get_boundaries(_ctx->_eng);
    coord_t _gripper = {._x = _cmd->from, ._y = boundaries._y};

    for (size_t _ii = boundaries._y; _ii > 2;)
    {
        aoc_2d_object_t _ObjInCol = NULL;
        _ObjInCol = aoc_engine_get_obj_by_position(_ctx->_eng, &_gripper);
        if (_ObjInCol)
        {
            aoc_2d_object_tracker_t *_ntracker = aoc_2d_object_tracker(_ObjInCol);
            if (_ntracker)
                ll_node_append(&_ctx->_BoxStack, CAST(struct ll_node_t *, _ntracker));
            else
                aoc_err("%s:%d aoc_2d_object_tracker failed", __FILE__, __LINE__);
        }
        else
            break;
        move_pos(_ctx->_eng, &_gripper, 1, AOC_DIR_UP);
    }

    size_t nGripped = 0;
    LL_FOREACH_LAST(_box, _ctx->_BoxStack)
    {
        aoc_2d_object_tracker_t *_ntracker = aoc_2d_object_tracker(CAST(aoc_2d_object_tracker_t *, _box)->data);
        if (_ntracker)
            ll_node_append(&_ctx->_grippedBoxes, CAST(struct ll_node_t *, _ntracker));
        else
            aoc_err("%s:%d aoc_2d_object_tracker failed", __FILE__, __LINE__);

        nGripped++;
        if (nGripped == _cmd->count)
            break;
    }

    aoc_engine_list_objects(_ctx->_eng);

    size_t _ll_size = aoc_ll_size(&_ctx->_grippedBoxes);
    if (_ll_size)
    {
        aoc_2d_object_t _grippedObj = NULL;
        char *_grippedReport = malloc(5 * _ll_size);
        char *_pGrippedReport = _grippedReport;
        if (!_grippedReport)
            return 0;
        memset(_grippedReport, '\0', 5 * _ll_size);
        LL_FOREACH(_node, _ctx->_grippedBoxes)
        {
            aoc_2d_object_tracker_t *tracker = CAST(aoc_2d_object_tracker_t *, _node);
            _grippedObj = tracker->data;
            _pGrippedReport += sprintf(_pGrippedReport, "%s", aoc_engine_get_obj_name(_grippedObj));
        }

        bool _all_blocked = false;
        while (!_all_blocked)
        {
            _all_blocked = true;
            LL_FOREACH(_node, _ctx->_grippedBoxes)
            {
                aoc_2d_object_tracker_t *tracker = CAST(aoc_2d_object_tracker_t *, _node);
                _grippedObj = tracker->data;
                tracker->_blocked = (0 != aoc_engine_step_object_and_redraw(_ctx->_eng, _grippedObj, AOC_DIR_UP, GREEN));
                if (!tracker->_blocked)
                    _all_blocked = false;
            }
            aoc_engine_list_objects(_ctx->_eng);
        }

        aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "gripped", _grippedReport);
        free(_grippedReport);
        return 0;
    }

    char _coord_str[4] = "";
    sprintf(_coord_str, "%3ld", _cmd->from);
    aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "nothing to grip at", _coord_str);
    return 1;
}

static int crate_change_lane(struct context_t *_ctx, command_t *_cmd)
{
    int ret = 0;
    aoc_2d_object_t _grippedObj = NULL;
    LL_FOREACH(_node, _ctx->_grippedBoxes)
    {
        _grippedObj = CAST(aoc_2d_object_tracker_t *, _node)->data;
        if (!_grippedObj)
            aoc_err("%s", "NULL pointer gripped box");

        size_t _ii = 0;
        for (size_t i = 0; (i < 100) && (aoc_engine_get_object_position(_ctx->_eng, _grippedObj)._x != _cmd->to); i++)
        {
            AOC_2D_DIR dir = aoc_engine_get_object_position(_ctx->_eng, _grippedObj)._x > _cmd->to ? AOC_DIR_LEFT : AOC_DIR_RIGHT;
            aoc_engine_step_object_and_redraw(_ctx->_eng, _grippedObj, dir, GREEN);
        }
        assert(_ii != 100 && "%s failed");
    }
    aoc_engine_list_objects(_ctx->_eng);

    char _coord_str[4] = "";
    sprintf(_coord_str, "%3ld", _cmd->from);
    aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "at lane %ld", _coord_str);
    return 0;
}

static int crate_deposit(struct context_t *_ctx, command_t *_cmd)
{
    aoc_2d_object_t _grippedObj = NULL;
    if (!aoc_ll_size(&_ctx->_grippedBoxes))
        return 0;

    char *_depositReport = malloc(5 * aoc_ll_size(&_ctx->_grippedBoxes));
    memset(_depositReport, '\0', 5 * aoc_ll_size(&_ctx->_grippedBoxes));
    char *_pDepositReport = _depositReport;

    size_t deposited = 0;

    LL_FOREACH_LAST(_node, _ctx->_grippedBoxes)
    {
        _grippedObj = CAST(aoc_2d_object_tracker_t *, _node)->data;
        _pDepositReport += sprintf(_pDepositReport, "%s", aoc_engine_get_obj_name(_grippedObj));
    }

    while (aoc_ll_size(&_ctx->_grippedBoxes) != deposited)
    {
        deposited = 0;
        LL_FOREACH_LAST(_node, _ctx->_grippedBoxes)
        {
            _grippedObj = CAST(aoc_2d_object_tracker_t *, _node)->data;
            if (aoc_engine_step_object_and_redraw(_ctx->_eng, _grippedObj, AOC_DIR_DOWN, GREEN))
                deposited++;
        }
        aoc_engine_list_objects(_ctx->_eng);

        aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "depositing", _depositReport);
    }
    LL_FOREACH_LAST_EXT(_node, _ctx->_grippedBoxes)
    {
        _grippedObj = CAST(aoc_2d_object_tracker_t *, _node)->data;
        draw_object(_ctx->_eng, _grippedObj, NULL);
    }

    aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "deposited", _depositReport);
    free(_depositReport);
    ll_free_all(&_ctx->_grippedBoxes, aoc_2d_object_tracker_free);
    ll_free_all(&_ctx->_BoxStack, aoc_2d_object_tracker_free);
    return 0;
}

static int crane_action(struct context_t *_ctx)
{
    size_t inc = 0;
    char _sCnt[4] = "";
    char _SFrom[4] = "";
    char _sDest[4] = "";

    LL_FOREACH(_node, _ctx->_cmds)
    {
        inc++;
        command_t *_cmd = CAST(command_t *, _node);
        sprintf(_sCnt, "%3ld", _cmd->count);
        sprintf(_SFrom, "%3ld", _cmd->from);
        sprintf(_sDest, "%3ld", _cmd->to);
        aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "move", _sCnt, "from", _SFrom, "to", _sDest);

        if (crate_lift(_ctx, _cmd))
            continue;
        if (crate_change_lane(_ctx, _cmd))
            continue;
        if (crate_deposit(_ctx, _cmd))
            continue;
    }
    ll_free_all(&_ctx->_cmds, free);
    char _uCmdCnt[4] = "";
    sprintf(_uCmdCnt, "%3ld", inc);
    aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, _uCmdCnt, "program finished commands executed");
    return 0;
}

static void aoc_spell_ans(struct context_t *_ctx)
{
    aoc_2d_object_t _grippedObj = NULL;
    size_t _colCount = aoc_engine_get_boundaries(_ctx->_eng)._x + 1;
    _ctx->spelling = malloc(_colCount);
    char *_p = _ctx->spelling;
    memset(_ctx->spelling, '\0', _colCount);
    for (size_t _col = 0; _col < _colCount; _col++)
    {
        command_t _spell1 = {.count = 1, .from = _col, .to = _col};
        crate_lift(_ctx, &_spell1);

        if (!_ctx->_grippedBoxes._first)
            continue;
        _grippedObj = CAST(aoc_2d_object_tracker_t *, _ctx->_grippedBoxes._first)->data;
        _p += sprintf(_p, "%c", aoc_engine_get_obj_name(_grippedObj)[1]);
        crate_deposit(_ctx, &_spell1);
    }
}