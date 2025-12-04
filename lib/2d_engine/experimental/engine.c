#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>
#include <math.h>
#include <fenv.h>
#include <fcntl.h>

size_t aoc_engine_get_XY_moves_between_objects(aoc_2d_engine_h _eng, aoc_2d_object_h _a, aoc_2d_object_h _b)
{
    coord_t _apos = aoc_engine_get_object_position(_eng, _a);
    coord_t _bpos = aoc_engine_get_object_position(_eng, _b);
    size_t _dX = HIGHEST(_apos._x, _bpos._x) - LOWEST(_apos._x, _bpos._x);
    size_t _dY = HIGHEST(_apos._y, _bpos._y) - LOWEST(_apos._y, _bpos._y);
    return (_dX + _dY);
}

size_t aoc_engine_get_XYD_moves_between_objects(aoc_2d_engine_h _eng, aoc_2d_object_h _a, aoc_2d_object_h _b)
{
    size_t ret = 0;

    coord_t _apos = aoc_engine_get_object_position(_eng, _a);
    coord_t _bpos = aoc_engine_get_object_position(_eng, _b);
    size_t _dX = HIGHEST(_apos._x, _bpos._x) - LOWEST(_apos._x, _bpos._x);
    size_t _dY = HIGHEST(_apos._y, _bpos._y) - LOWEST(_apos._y, _bpos._y);

    size_t _slope = 0;
    if (_dX)
    {
        assert(0 != _dX && "division by 0");
        _slope = _dY / _dX;
    }
    else
        _slope = _dY ? 1 : 0;
    _slope = HIGHEST(_slope, 1);
    coord_t _start = {._x = LOWEST(_apos._x, _bpos._x), ._y = LOWEST(_apos._y, _bpos._y)};
    coord_t _end = {._x = HIGHEST(_apos._x, _bpos._x), ._y = HIGHEST(_apos._y, _bpos._y)};

    if ((0 == _dY) && (0 == _dX))
        return 0;

    for (; (_start._x < _end._x) || (_start._y < _end._y); _start._x += _dX ? 1 : 0)
    {
        _start._y += _slope;
        ret++;
    }
    return ret;
}

int aoc_engine_move_object_and_redraw(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t steps, AOC_2D_DIR dir)
{
    int ret = aoc_engine_object_fit_detect(_eng, _obj, steps, dir);
    if (ret)
        return ret;

    ret = aoc_engine_object_fit_detect(_eng, _obj, steps, dir);
    if (ret)
        return ret;

    if (!_eng || !_obj || dir >= AOC_DIR_MAX)
        return EINVAL;

    aoc_engine_erase_object(_eng, _obj);
    dll_node_h _part_node;
    part_h _sym;
    LL_FOREACH_EXT(_part_node, _obj->_parts)
    {
        _sym = CAST(part_h, _part_node);
        move_within_coord(_eng, &_sym->_pos, steps, dir);
    }
    move_within_window(_eng, &_obj->_pos, steps, dir);
    aoc_engine_draw_object(_eng, _obj, NULL);
    engine_put_cursor_in_footer_area(_eng);
}


int aoc_engine_move_one_step_towards(aoc_2d_engine_h _eng, aoc_2d_object_h _a, coord_t _pos)
{
    int ret = 0;
    coord_t current = aoc_engine_get_object_position(_eng, _a);
    if (current._x < _pos._x)
        ret = aoc_engine_step_object(_eng, _a, AOC_DIR_RIGHT, NULL);
    if (current._x > _pos._x && !ret)
        ret = aoc_engine_step_object(_eng, _a, AOC_DIR_LEFT, NULL);
    if (current._y < _pos._y && !ret)
        ret = aoc_engine_step_object(_eng, _a, AOC_DIR_DOWN, NULL);
    if (current._y > _pos._y && !ret)
        ret = aoc_engine_step_object(_eng, _a, AOC_DIR_UP, NULL);
    return ret;
}

int aoc_engine_try_move_object_and_redraw(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t steps, AOC_2D_DIR dir)
{
    int ret = aoc_engine_object_fit_detect(_eng, _obj, steps, dir);
    if (ret)
        return ret;

    if (!_eng || !_obj || dir >= AOC_DIR_MAX)
        return EINVAL;

    aoc_engine_erase_object(_eng, _obj);
    dll_node_h _part_node;
    part_h _sym;
    LL_FOREACH_EXT(_part_node, _obj->_parts)
    {
        _sym = CAST(part_h, _part_node);
        move_within_window(_eng, &_sym->_pos, steps, dir);
    }
    move_within_window(_eng, &_obj->_pos, steps, dir);
    aoc_engine_draw_object(_eng, _obj, NULL);
    engine_put_cursor_in_footer_area(_eng);
    return 0;
}


int aoc_engine_move_object(aoc_2d_engine_h eng, aoc_2d_object_h obj, move_t *move)
{
    if (!eng || !obj)
        return EINVAL;
    aoc_engine_move_object_and_redraw(eng, obj, move, NULL);
    engine_put_cursor_in_footer_area(eng);
}