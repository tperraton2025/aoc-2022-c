#include "part1.h"

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR, _blk->_name);
    _blk->_data = malloc(sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;

    struct context *_ctx = CTX_CAST(_blk->_data);

    _ctx->result = 0;

    _ctx->_pos._x = 1;
    _ctx->_pos._y = 1;

    _ctx->_grippedBox = NULL;

    dll_head_init(&_ctx->_cmds);
    dll_head_init(&_ctx->_columns);
    dll_head_init(&_ctx->_parsers);

    parser_append(&_ctx->_parsers, &blockparser, _ctx);
    parser_append(&_ctx->_parsers, &commandparser, &_ctx->_cmds);

    bool _drawingenabled = true;
    size_t delay = 0;
    coord_t _prelcoordmaxima = {._x = 2, ._y = 2};

    if (argc >= 2)
        for (int _iarg = 0; _iarg < argc; _iarg++)
        {
            if (0 == strcmp(argv[_iarg], "--nodraw"))
                _drawingenabled = false;
            if ((0 == strcmp(argv[_iarg], "--draw-delay")) && argc > (_iarg + 1))
                sscanf(argv[_iarg + 1], "%3lu", &delay);
        }

    _ctx->_eng = engine_create(&_prelcoordmaxima, '~', 0);

    if (!_ctx->_eng)
        goto cleanup;

    if (!_drawingenabled)
        engine_deactivate_drawing(_ctx->_eng);
    eng_set_refresh_delay(_ctx->_eng, delay);

    return 0;

cleanup:
    FREE(_blk->_data);
error:
    return ENOMEM;
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
    char test[16] = {0};

    aoc_engine_extend_one_direction(_ctx->_eng, 20, AOC_DIR_UP);
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
    FREE(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;

static int crate_lift(struct context *_ctx, command_t *_cmd)
{
    coordboundaries_t boundaries = aoc_engine_get_parts_boundaries(_ctx->_eng);
    for (size_t _ii = 0; _ii <= boundaries._max._y; _ii++)
    {
        coord_t _gripper = {._x = _cmd->from, ._y = _ii};
        _ctx->_grippedBox = aoc_engine_get_obj_by_position(_ctx->_eng, &_gripper);
        if (_ctx->_grippedBox)
        {
            aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "gripped", aoc_engine_get_obj_name(_ctx->_grippedBox));
            while (!aoc_engine_step_object(_ctx->_eng, _ctx->_grippedBox, 1LU, AOC_DIR_UP, GREEN))
                aoc_engine_prompt_obj_list(_ctx->_eng);
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
    for (; (_ii < 100) && (_ctx->_pos._x != _cmd->to); _ii++)
    {
        AOC_2D_DIR dir = _ctx->_pos._x > _cmd->to ? AOC_DIR_LEFT : AOC_DIR_RIGHT;
        _ctx->_pos._x = _ctx->_pos._x > _cmd->to ? _ctx->_pos._x - 1 : _ctx->_pos._x + 1;

        if (aoc_engine_step_object(_ctx->_eng, _ctx->_grippedBox, 1LU, dir, GREEN))
        {
            aoc_engine_prompt_extra_stats_as_err(_ctx->_eng, "moving %s failed", aoc_engine_get_obj_name(_ctx->_grippedBox));
        }
        aoc_engine_prompt_obj_list(_ctx->_eng);

        char _dest_col_str[4] = "";
        sprintf(_dest_col_str, "%3ld", _cmd->to);
        aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "at lane %ld", _dest_col_str);
    }
    assert(_ii != 100 && "%s failed");
    return 0;
}

static int crate_deposit(struct context *_ctx, command_t *_cmd)
{
    while (!aoc_engine_step_object(_ctx->_eng, _ctx->_grippedBox, 1LU, AOC_DIR_DOWN, GREEN))
    {
        aoc_engine_prompt_obj_list(_ctx->_eng);
        aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "depositing", aoc_engine_get_obj_name(_ctx->_grippedBox));
    }
    aoc_engine_draw_object(_ctx->_eng, _ctx->_grippedBox, NULL);
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
        aoc_engine_prompt_obj_list(_ctx->_eng);
    }
    dll_free_all(&_ctx->_cmds, free);
    dll_free_all(&_ctx->_columns, free);

    char _uCmdCnt[4] = "";
    sprintf(_uCmdCnt, "%3ld", inc);
    aoc_engine_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, _uCmdCnt, "program finished commands executed");
    return 0;
}

static void aoc_spell_ans(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);
    size_t _colCount = _ctx->_columns._size + 1;
    _ctx->spelling = malloc(_colCount);
    char *_p = _ctx->spelling;
    memset(_ctx->spelling, '\0', _colCount);
    for (size_t _col = 0; _col < _colCount; _col++)
    {
        command_t _spell1 = {.count = 1, .from = _col * 4, .to = _col * 4};
        crate_lift(_ctx, &_spell1);
        if (!_ctx->_grippedBox)
            continue;
        _p += sprintf(_p, "%c", aoc_engine_get_obj_name(_ctx->_grippedBox)[1]);
        crate_deposit(_ctx, &_spell1);
    }
    aoc_ans("AOC 2022 %s solution is %s", _blk->_name, _ctx->spelling);

    FREE(_ctx->spelling);
}

static int parsecommand(void *arg, char *_str)
{
    dll_head_h _cmds = (dll_head_h)arg;
    int match = 0;
    int ret = 0;
    command_t _nc = {0};
    match = sscanf(_str, "move %ld from %ld to %ld", &_nc.count, &_nc.from, &_nc.to);
    if (3 == match)
    {
        _nc.from *= 4;
        _nc.to *= 4;
        command_t *_pnc = command_ctor(&_nc);
        if (!_pnc)
            return ENOMEM;
        ret = dll_node_append(_cmds, CAST(dll_node_h, _pnc));
        if (ret)
        {
            FREE(_pnc);
            return ENOMEM;
        }
        return 0;
    }
    return -1;
}

static int parseblock(void *arg, char *_str)
{
    aoc_context_h _ctx = (aoc_context_h)arg;
    int match = 0;
    int ret = 0;
    size_t _len = strnlen(_str, MAX_LINE_LEN);
    if (_len <= 3)
        return -1;
    _ctx->_pos._x = 1;

    char _buf[] = "[A]";

    for (char *_ss = strchr(_str, '['); _ss; _ss = strchr(_ss + 1, '['))
    {
        _ctx->_pos._x = (_ss - _str);
        memcpy(_buf, _ss, 3);
        match += sscanf(_buf, "[%c]", &_buf[1]);
        if (!match)
            continue;
        if (N_IN_RANGE(_buf[1], 'A', 'Z'))
        {
            aoc_2d_object_h _nobj = aoc_engine_object(_ctx->_eng, _buf, &_ctx->_pos, _buf, OBJ_PROPERTY_MOBILE);
            ret = aoc_engine_append_obj(_ctx->_eng, _nobj);
            if (ret)
                break;
        }
        if (!dll_find_node_by_property(&_ctx->_columns, &_ctx->_pos._x, has_same_column))
        {
            coord_tracker_t *_trck = coord_tracker();
            _trck->_coord._x = _ctx->_pos._x;
            dll_node_sorted_insert(&_ctx->_columns, NODE_CAST(_trck), highest_column);
        }
        _ctx->_pos._x++;
    }
    _ctx->_pos._y++;
    return match ? 0 : -1;
}
