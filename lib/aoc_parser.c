#include "aoc_helpers.h"
#include "engine.h"
#include <aoc_parser.h>
#include <assert.h>

void aoc_parser_free(void *_data)
{
    aoc_parser_t _pars = CAST(aoc_parser_t, _data);
    FREE(_pars->_name);
    FREE(_pars);
}

int aoc_parser_append(aoc_linked_list_handle_t _ll, const aoc_parser_t const _prs)
{
    int ret = 0;
    if (!_prs || !_ll)
        return EINVAL;

    aoc_parser_t _nprs = malloc(sizeof(struct parser_t));
    if (!_nprs)
        return ENOMEM;

    _nprs->_name = malloc(sizeof(char) * strnlen(_prs->_name, ABSOLUTE_MAX_NAME_LEN));
    if (!_nprs->_name)
        ret = ENOMEM;
    if (ret)
        goto error;

    sprintf(_nprs->_name, NAME_FMT, _prs->_name);

    _nprs->_parseRegx = _prs->_parseRegx;
    _nprs->_arg = _prs->_arg;

    ret = ll_node_append(_ll, NODE_CAST(_nprs));
    if (ret)
        goto error;

    return 0;
error:
    if (_nprs->_name)
        FREE(_nprs->_name);
    if (_nprs)
        FREE(_nprs);

    return ret;
}
