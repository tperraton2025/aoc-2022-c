#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <aoc_linked_list.h>
#include <stdint.h>
#include "partX.h"
#include <aoc_parser.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context_t
{
    aoc_2d_engine_t _eng;
    aoc_2d_object_t _grippedBox;
    coord_t _pos;
    struct ll_context_t _cmds;
    char* spelling;
    int result;
};
 

#define CTX_CAST(_p) ((struct context_t *)_p)

static int parse_all(aoc_context_t _ctx, char *_str);
static int block_parser(aoc_context_t _ctx, char *_str);
static int command_parser(aoc_context_t _ctx, char *_str);

static int crate_lift(aoc_context_t _ctx, command_t *_cmd);
static int crate_change_lane(aoc_context_t _ctx, command_t *_cmd);
static int crate_deposit(aoc_context_t _ctx, command_t *_cmd);
static int crane_action(aoc_context_t _ctx);

static void aoc_spell_ans(struct context_t *_ctx);

static struct parser_t _block = {._name = "block", ._parseRegx = "[%c]", ._func = block_parser};
static struct parser_t _cmd = {._name = "command", ._parseRegx = "move %d from %d to %d", ._func = command_parser};
static struct parser_t *_parsers[] = {&_block, &_cmd};

static int parse_all(aoc_context_t _ctx, char *_str)
{
    for (size_t _ii = 0; _ii < ARRAY_DIM(_parsers); _ii++)
    {
        struct parser_t *_prs = CAST(struct parser_t *, _parsers[_ii]);
        _prs->_parsed += _prs->_func(_ctx, _str);
        if (!_prs->_parsed)
            break;
    }
    return 0;
}
 

static int block_parser(aoc_context_t _ctx, char *_str)
{
    int match = 0;
    int ret = 0;
    size_t _len = strnlen(_str, MAX_LINE_LEN);
    if (_len <= 3)
        return -1;
    _ctx->_pos._x = 1;

    char _buf[] = "[A]";

    for (char *_ss = strchr(_str, '['); _ss; _ss = strchr(_ss + 1, '['))
    {
        _ctx->_pos._x = (_ss - _str) / 4 + 1;
        memcpy(_buf, _ss, 3);
        match += sscanf(_buf, "[%c]", &_buf[1]);
        if (!match)
            continue;
        if (N_IN_RANGE(_buf[1], 'A', 'Z'))
        {
            aoc_2d_object_t _nobj = eng_obj_create(_ctx->_eng, _buf, &_ctx->_pos, _buf, OBJ_PROPERTY_MOBILE);
            ret = aoc_engine_append_obj(_ctx->_eng, _nobj);
            if (ret)
                break;
        }
        _ctx->_pos._x++;
    }
    _ctx->_pos._y++;
    return match ? 0 : -1;
}

static int command_parser(aoc_context_t _ctx, char *_str)
{
    int match = 0;
    int ret = 0;
    command_t _nc = {0};
    match = sscanf(_str, "move %ld from %ld to %ld", &_nc.count, &_nc.from, &_nc.to);
    if (3 == match)
    {
        command_t *_pnc = command_ctor(&_nc);
        if (!_pnc)
            return ENOMEM;
        ret = ll_node_append(&_ctx->_cmds, CAST(struct ll_node_t *, _pnc));
        if (ret)
        {
            free(_pnc);
            return ENOMEM;
        }
        return 0;
    }
    return -1;
} 