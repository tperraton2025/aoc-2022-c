#include "aoc_helpers.h"
#include "engine.h"
#include <aoc_parser.h>
#include <assert.h>

void parser_free(void *_data)
{
    parser_h _pars = CAST(parser_h, _data);
    FREE(_pars->_name);
    FREE(_pars);
}

int parser_append(dll_head_h head, parser_h parser, void *arg)
{
    int ret = 0;
    if (!parser || !head)
        return EINVAL;

    assert(arg && "NULL pointer");
    parser->arg = arg;
    ret = dll_node_append(head, &parser->_node);
    if (ret)
        goto error;

    return 0;
error:
    return ret;
}

int parse_all(dll_head_h head, char *str)
{
    LL_FOREACH_P(_parsernode, head)
    {
        parser_h _parser = (parser_h)_parsernode;
        _parser->_parsed = _parser->_func(_parser->arg, str);
        if (!_parser->_parsed)
            break;
    }
    return 0;
}

void parser_list_free(parser_h *list){

}
