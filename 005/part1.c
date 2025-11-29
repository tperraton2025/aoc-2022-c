#include "part1.h"

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    _blk->_data = malloc(sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;

    struct context *_ctx = CTX_CAST(_blk->_data);

    _ctx->result = 0;

    _ctx->_pos._x = 1;
    _ctx->_pos._y = 1;

    _ctx->_grippedBox = NULL;

    dll_head_init(&_ctx->_cmds);

    coord_t _reso = {3, 1};
    coord_t _spce = {1, 0};
    _ctx->_eng = engine_create(_reso, _spce, '~');
    if (_ctx->_eng)
        goto success;
    goto cleanup;

success:
    size_t delay = 0;
    if (argc >= 2)
        for (int _iarg = 0; _iarg < argc; _iarg++)
        {
            if (0 == strcmp(argv[_iarg], "--nodraw"))
                engine_deactivate_drawing(_ctx->_eng);
            if ((0 == strcmp(argv[_iarg], "--draw-delay")) && argc > (_iarg + 1))
                sscanf(argv[_iarg + 1], "%3lu", &delay);
        }

    eng_set_refresh_delay(_ctx->_eng, delay);

    return 0;

cleanup:
    free(_blk->_data);
error:
    return ENOMEM;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    parse_all(_ctx, _blk->_str);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    char test[16] = {0};

    aoc_engine_resize_one_direction(_ctx->_eng, 50, AOC_DIR_DOWN);
    engine_draw(_ctx->_eng);
    crane_action(_ctx);
    aoc_spell_ans(_blk);

    int ret = _ctx->result;
    return ret;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);

    engine_free(_ctx->_eng);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;

static int crate_lift(struct context *_ctx, command_t *_cmd)
{
    coord_t boundaries = aoc_engine_get_boundaries(_ctx->_eng);
    for (size_t _ii = 0; _ii <= boundaries._y; _ii++)
    {
        coord_t _gripper = {._x = _cmd->from, ._y = _ii};
        _ctx->_grippedBox = aoc_engine_get_obj_by_position(_ctx->_eng, &_gripper);
        if (_ctx->_grippedBox)
        {
            while (!aoc_engine_step_object_and_redraw(_ctx->_eng, _ctx->_grippedBox, AOC_DIR_UP, GREEN))
            {
                aoc_engine_list_objects(_ctx->_eng);
                aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "gripped", aoc_engine_get_obj_name(_ctx->_grippedBox));
            }
            return 0;
        }
    }
    char _coord_str[4] = "";
    sprintf(_coord_str, "%3ld", _cmd->from);
    aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "nothing to grip at", _coord_str);
    return 1;
}

static int crate_change_lane(struct context *_ctx, command_t *_cmd)
{
    int ret = 0;
    if (!_ctx->_grippedBox)
        aoc_err("%s", "NULL pointer gripped box");
    size_t _ii = 0;
    for (; (_ii < 100) && (aoc_engine_get_object_position(_ctx->_eng, _ctx->_grippedBox)._x != _cmd->to); _ii++)
    {
        AOC_2D_DIR dir = aoc_engine_get_object_position(_ctx->_eng, _ctx->_grippedBox)._x > _cmd->to ? AOC_DIR_LEFT : AOC_DIR_RIGHT;
        aoc_engine_step_object_and_redraw(_ctx->_eng, _ctx->_grippedBox, dir, GREEN);
        aoc_engine_list_objects(_ctx->_eng);

        char _dest_col_str[4] = "";
        sprintf(_dest_col_str, "%3ld", _cmd->to);
        aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "at lane %ld", _dest_col_str);
    }
    assert(_ii != 100 && "%s failed");
    return 0;
}

static int crate_deposit(struct context *_ctx, command_t *_cmd)
{
    while (!aoc_engine_step_object_and_redraw(_ctx->_eng, _ctx->_grippedBox, AOC_DIR_DOWN, GREEN))
    {
        aoc_engine_list_objects(_ctx->_eng);
        aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "depositing", aoc_engine_get_obj_name(_ctx->_grippedBox));
    }
    draw_object(_ctx->_eng, _ctx->_grippedBox, NULL);
    return 0;
}

static int crane_action(struct context *_ctx)
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

        for (size_t _repeat = 0; _repeat < _cmd->count; _repeat++)
        {
            if (crate_lift(_ctx, _cmd))
                continue;
            if (crate_change_lane(_ctx, _cmd))
                continue;
            if (crate_deposit(_ctx, _cmd))
                continue;
        }
        aoc_engine_list_objects(_ctx->_eng);
    }
    dll_free_all(&_ctx->_cmds, free);

    char _uCmdCnt[4] = "";
    sprintf(_uCmdCnt, "%3ld", inc);
    aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, _uCmdCnt, "program finished commands executed");
    return 0;
}

static void aoc_spell_ans(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);
    size_t _colCount = aoc_engine_get_boundaries(_ctx->_eng)._x + 1;
    _ctx->spelling = malloc(_colCount);
    char *_p = _ctx->spelling;
    memset(_ctx->spelling, '\0', _colCount);
    for (size_t _col = 0; _col < _colCount; _col++)
    {
        command_t _spell1 = {.count = 1, .from = _col, .to = _col};
        crate_lift(_ctx, &_spell1);
        if (!_ctx->_grippedBox)
            continue;
        _p += sprintf(_p, "%c", aoc_engine_get_obj_name(_ctx->_grippedBox)[1]);
        crate_deposit(_ctx, &_spell1);
    }
    aoc_ans("AOC 2022 %s solution is %s", _blk->_name, _ctx->spelling);

    free(_ctx->spelling);
}