#include "aoc_ranges.h"
#include <errno.h>

DLL_NODE_CTOR(rangenode_t, rangenode_ctor);

rangelist_h rangelistinit()
{
    rangelist_h nrange = NULL;
    TRY_RAII_MALLOC(nrange, sizeof(rangelist_t));
    return nrange;
}

int tryinsertrange(rangelist_h list, rangenode_h newrangenode)
{

#ifdef AOC_RANGE_VERBOSE_MODE
    static char rangestringa[] = "[" STR(__SIZE_MAX__) "," STR(__SIZE_MAX__) "]";
    static char rangestringb[] = "[" STR(__SIZE_MAX__) "," STR(__SIZE_MAX__) "]";
#endif
    bool _additself = false;
    rangenode_h _toreeval = NULL;
#ifdef AOC_RANGE_VERBOSE_MODE
    aoc_info("eval %s", str_rangenode(newrangenode));
#endif
    LL_FOREACH(_it, list->_head)
    {
        rangenode_h _itrnode = (rangenode_h)_it;
        range_h _itrangeh = &_itrnode->_range;
        range_h _newrangeh = &newrangenode->_range;

        if ((_it->_obsolete))
            continue;

        if (_itrangeh == _newrangeh)
        {
            _additself = true;
            continue;
        }

        if (A_AROUND_B_P(_itrangeh, _newrangeh))
        {
            newrangenode->_node._obsolete = true;
#ifdef AOC_RANGE_VERBOSE_MODE
            sprintf(rangestringa, "%s", str_rangenode(newrangenode));
            sprintf(rangestringb, "%s", str_rangenode(_itrnode));
            aoc_ans("%s obsoleted by %s, the end\t\t\t:)", rangestringa, rangestringb);
#endif
            return EEXIST;
        }
        else if A_AROUND_B_P (_newrangeh, _itrangeh)
        {
            newrangenode->_node._obsolete = true;
#ifdef AOC_RANGE_VERBOSE_MODE
            sprintf(rangestringa, "%s", str_rangenode(newrangenode));
            sprintf(rangestringb, "%s", str_rangenode(_itrnode));
            aoc_warn("%s overlapping %s", rangestringa, rangestringb);
#endif
            _itrangeh->_max = _newrangeh->_max;
            _itrangeh->_min = _newrangeh->_min;
            _toreeval = _itrnode;
#ifdef AOC_RANGE_VERBOSE_MODE
            sprintf(rangestringa, "%s", str_rangenode(newrangenode));
            sprintf(rangestringb, "%s", str_rangenode(_itrnode));
            aoc_ans("%s obsoleted by %s", rangestringa, rangestringb);
#endif
            tryinsertrange(list, _toreeval);
            return EEXIST;
        }
        else if (N_IN_RANGE_P(_newrangeh->_max, _itrangeh))
        {
            newrangenode->_node._obsolete = true;
#ifdef AOC_RANGE_VERBOSE_MODE
            sprintf(rangestringa, "%s", str_rangenode(newrangenode));
            sprintf(rangestringb, "%s", str_rangenode(_itrnode));
            aoc_warn("%s overlapping %s", rangestringa, rangestringb);
#endif
            _itrangeh->_min = _newrangeh->_min;
            _toreeval = _itrnode;
#ifdef AOC_RANGE_VERBOSE_MODE
            sprintf(rangestringa, "%s", str_rangenode(newrangenode));
            sprintf(rangestringb, "%s", str_rangenode(_itrnode));
            aoc_ans("%s obsoleted by %s", rangestringa, rangestringb);
#endif
            tryinsertrange(list, _toreeval);
            return EEXIST;
        }
        else if (N_IN_RANGE_P(_newrangeh->_min, _itrangeh))
        {
            newrangenode->_node._obsolete = true;
#ifdef AOC_RANGE_VERBOSE_MODE
            sprintf(rangestringa, "%s", str_rangenode(newrangenode));
            sprintf(rangestringb, "%s", str_rangenode(_itrnode));
            aoc_warn("%s overlapping %s", rangestringa, rangestringb);
#endif
            _itrangeh->_max = _newrangeh->_max;
            _toreeval = _itrnode;
#ifdef AOC_RANGE_VERBOSE_MODE
            sprintf(rangestringa, "%s", str_rangenode(newrangenode));
            sprintf(rangestringb, "%s", str_rangenode(_itrnode));
            aoc_ans("%s obsoleted by %s", rangestringa, rangestringb);
#endif
            tryinsertrange(list, _toreeval);
            return EEXIST;
        }
        else
            continue;
    }
    if (!newrangenode->_node._obsolete && !_additself)
    {
        rangenode_h _newrange = rangenode_ctor();
        if (!_newrange)
            return ENOMEM;
        _newrange->_range._min = newrangenode->_range._min;
        _newrange->_range._max = newrangenode->_range._max;
#ifdef AOC_RANGE_VERBOSE_MODE
        aoc_ans("adding %s, the end\t\t\t:)", str_rangenode(newrangenode));
#endif
        return dll_node_append(&list->_head, &_newrange->_node);
    }
}

static char rangestring[] = "[" STR(__SIZE_MAX__) "," STR(__SIZE_MAX__) "]";

char *str_rangenode(rangenode_h rangen)
{
    sprintf(rangestring, "[%lu,%lu]", rangen->_range._min, rangen->_range._max);
    return rangestring;
}