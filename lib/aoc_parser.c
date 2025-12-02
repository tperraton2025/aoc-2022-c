#include "aoc_helpers.h"
#include "engine.h"
#include <aoc_parser.h>
#include <assert.h>

void parser_free(void *_data)
{
    parser_h _pars = CAST(parser_h, _data);
    FREE_AND_CLEAR_P(_pars->_name);
    FREE_AND_CLEAR_P(_pars);
}

int parser_append(dll_head_h _ll, parser_h parser, void *arg)
{
    int ret = 0;
    if (!parser || !_ll)
        return EINVAL;

    parser->arg = arg;
    ret = dll_node_append(_ll, &parser->_node);
    if (ret)
        goto error;

    return 0;
error:
    return ret;
}

int parse_all(dll_head_h _ll, char *str)
{
    LL_FOREACH_P(_parsernode, _ll)
    {
        parser_h _parser = (parser_h)_parsernode;
        _parser->_parsed = _parser->_func(_parser->arg, str);
        if (!_parser->_parsed)
            break;
    }
    return 0;
}