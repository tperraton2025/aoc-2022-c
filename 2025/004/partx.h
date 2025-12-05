#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <aoc_parser.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

static int parserolls(void *arg, char *str)
{
    int _ret = 0;
    aoc_2d_engine_h _eng = (aoc_2d_engine_h)arg;

    char *_start = str;
    _start = strchr(_start, '@');
    while (_start && !_ret)
    {
        coord_t _pos = {._x = _start - str, ._y = engine_last_line(_eng)};
        if (_start)
            _start++;

        char _objname[] = "roll9999999";
        sprintf(_objname, "roll%-3s", strobjcnt(_eng));
        aoc_2d_object_h _objh = aoc_engine_object(_eng, _objname, &_pos, "@", OBJ_PROPERTY_STATIC);
        if (!_objh)
            _ret = ENOMEM;
        if (!_ret)
            _ret = aoc_engine_append_obj(_eng, _objh);
        _start = strchr(_start, '@');
    }
    engine_add_line(_eng);
    return _ret;
}

static struct parser blockparser = {._name = "roll parsers", ._func = parserolls};

static dll_head_h enumeraterollsatproximity(dll_head_h allpos)
{
    dll_head_h _tomark = malloc(sizeof(dll_head_t));
    dll_head_init(_tomark);

    LL_FOREACH_P(_posn, allpos)
    {
        size_t _limit = 0;
        coord_tracker_t _pos = {0};
        coord_tracker_h _trkh = (coord_tracker_h )_posn;
        coord_t *_posh = &_trkh->_coord;

        size_t startx = _posh->_x ? _posh->_x - 1 : _posh->_x;
        size_t starty = _posh->_y ? _posh->_y - 1 : _posh->_y;


        for (_pos._coord._x = startx; _pos._coord._x < (_posh->_x + 2); _pos._coord._x++)
        {
            for (_pos._coord._y = starty; _pos._coord._y < (_posh->_y + 2); _pos._coord._y++)
            {
                if ((_pos._coord._x == _posh->_x) && (_pos._coord._y == _posh->_y)) 
                    continue; 
                if (dll_find_node_by_property(allpos, &_pos, coord_compare))
                    _limit += 1;
            }
        }
        if (_limit < 4)
        {
            coord_tracker_h _ntr = coordtracker_ctor();
            _ntr->_coord._x = _posh->_x;
            _ntr->_coord._y = _posh->_y;
            dll_node_append(_tomark, &_ntr->_node);
        }
    }
    return _tomark;
}

static void markfreerolls(struct context *_ctx);