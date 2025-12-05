#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>

static char _strposbuf[] = "[00000x00000]";

int move_cursor_until(struct ascii_2d_engine *_eng, AOC_2D_DIR _dir, size_t _steps, coord_t *limit)
{
    int ret = 0;
    switch (_dir)
    {
    case AOC_DIR_UP:
        if (_eng->_cursor._y == limit->_y)
            return ERANGE;
        _eng->_cursor._y -= _steps;
        break;
    case AOC_DIR_DOWN:
        if ((_eng->_cursor._y + _steps) > limit->_y)
            return ERANGE;
        _eng->_cursor._y += _steps;
        break;
    case AOC_DIR_LEFT:
        if (_eng->_cursor._x == limit->_x)
            return ERANGE;
        _eng->_cursor._x -= _steps;
        break;
    case AOC_DIR_RIGHT:
        if ((_eng->_cursor._x + _steps) > limit->_x)
            return ERANGE;
        _eng->_cursor._x += _steps;
        break;

    default:
        return EINVAL;
    }
    return ret;
}

int move_within_coord(aoc_2d_engine_h _eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir)
{
    switch (_dir)
    {
    case AOC_DIR_UP:
        if (!N_BETWEEN_AB(_pos->_y - _steps, _eng->_coordlimits._min._y, _eng->_coordlimits._max._y))
            return ERANGE;
        _pos->_y -= _steps;
        break;
    case AOC_DIR_DOWN:
        if (!N_BETWEEN_AB(_pos->_y + _steps, _eng->_coordlimits._min._y, _eng->_coordlimits._max._y))
            return ERANGE;
        _pos->_y += _steps;
        break;
    case AOC_DIR_LEFT:
        if (!N_BETWEEN_AB(_pos->_x - _steps, _eng->_coordlimits._min._x, _eng->_coordlimits._max._x))
            return ERANGE;
        _pos->_x -= _steps;
        break;
    case AOC_DIR_RIGHT:
        if (!N_BETWEEN_AB(_pos->_x + _steps, _eng->_coordlimits._min._x, _eng->_coordlimits._max._x))
            return ERANGE;
        _pos->_x += _steps;
        break;

    default:
        return EINVAL;
    }
    return 0;
}

int move_within_window(aoc_2d_engine_h _eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir)
{
    switch (_dir)
    {
    case AOC_DIR_UP:
        if (!N_BETWEEN_AB(_pos->_y - _steps, _eng->_drawlimits._min._y, _eng->_drawlimits._max._y))
            return ERANGE;
        _pos->_y -= _steps;
        break;
    case AOC_DIR_DOWN:
        if (!N_BETWEEN_AB(_pos->_y + _steps, _eng->_drawlimits._min._y, _eng->_drawlimits._max._y))
            return ERANGE;
        _pos->_y += _steps;
        break;
    case AOC_DIR_LEFT:
        if (!N_BETWEEN_AB(_pos->_x - _steps, _eng->_drawlimits._min._x, _eng->_drawlimits._max._x))
            return ERANGE;
        _pos->_x -= _steps;
        break;
    case AOC_DIR_RIGHT:
        if (!N_BETWEEN_AB(_pos->_x + _steps, _eng->_drawlimits._min._x, _eng->_drawlimits._max._x))
            return ERANGE;
        _pos->_x += _steps;
        break;

    default:
        return EINVAL;
    }
    return 0;
}

int is_position_in_box(aoc_2d_engine_h _eng, coord_t *_pos)
{
    if (!N_BETWEEN_AB(_pos->_y, _eng->_coordlimits._min._y, _eng->_coordlimits._max._y))
        return ERANGE;
    if (!N_BETWEEN_AB(_pos->_x, _eng->_coordlimits._min._x, _eng->_coordlimits._max._x))
        return ERANGE;
    return 0;
}

int put_pos(aoc_2d_engine_h _eng, coord_t *_pos, coord_t *_npos)
{
    int _ret = is_position_in_box(_eng, _npos);
    if (_ret)
        return _ret;

    _pos->_y = _npos->_y;
    _pos->_x = _npos->_x;

    return 0;
}

int put_part(aoc_2d_engine_h _eng, part_h _sym, coord_t *_delta)
{
    if (!N_BETWEEN_AB(_delta->_y, _eng->_coordlimits._min._x, _eng->_coordlimits._max._y))
        return ERANGE;
    if (!N_BETWEEN_AB(_delta->_x, _eng->_coordlimits._min._x, _eng->_coordlimits._max._x))
        return ERANGE;
    return 0;
}

int engine_put_cursor_in_footer_area(struct ascii_2d_engine *_eng)
{
    if (!_eng)
        return EINVAL;
    printf(MCUR_FMT ERASE_LINE_FROM_CR, _eng->_drawlimits._max._y + 1LU, 1LU);
    return 0;
}

int engine_cursor_log(struct ascii_2d_engine *_eng)
{
    if (!_eng)
        return EINVAL;
    printf(MCUR_FMT ERASE_LINE_FROM_CR, _eng->_drawlimits._max._y + 2LU, 0LU);
    return 0;
}

int engine_cursor_stats(struct ascii_2d_engine *_eng)
{
    if (!_eng)
        return EINVAL;
    _eng->_statLine = 2;
    if (_eng->_enabledraw)
        printf(MCUR_FMT ERASE_LINE_FROM_CR, _eng->_statLine++, _eng->_drawlimits._max._x + 1);
    return 0;
}

int engine_cursor_user_stats(struct ascii_2d_engine *_eng)
{
    if (!_eng)
        return EINVAL;
    _eng->_statLine = _eng->_PrivStatLine;
    if (_eng->_enabledraw)
        printf(MCUR_FMT ERASE_LINE_FROM_CR, _eng->_statLine++, _eng->_drawlimits._max._x + 1);
    return 0;
}

int engine_cursor_private_next_stats(struct ascii_2d_engine *_eng)
{
    if (!_eng)
        return EINVAL;
    if (_eng->_enabledraw)
        printf(MCUR_FMT ERASE_LINE_FROM_CR, _eng->_statLine++, _eng->_drawlimits._max._x + 1);
    _eng->_PrivStatLine = _eng->_statLine + 1;
    return 0;
}

int engine_cursor_user_next_stats(struct ascii_2d_engine *_eng)
{
    if (!_eng)
        return EINVAL;
    if (_eng->_enabledraw)
        printf(MCUR_FMT ERASE_LINE_FROM_CR, _eng->_statLine++, _eng->_drawlimits._max._x + 1);
    else
        printf(MCUR_FMT ERASE_LINE_FROM_CR, 4LU, 2LU);

    _eng->_statLine = _eng->_statLine >= _eng->_drawlimits._max._y ? _eng->_PrivStatLine : _eng->_statLine;
    return 0;
}

int engine_cursor_exit_drawing_area(struct ascii_2d_engine *_eng)
{
    if (!_eng)
        return EINVAL;
    printf(MCUR_FMT ERASE_LINE_FROM_CR, _eng->_drawlimits._max._y + 10, 0LU);
    return 0;
}

/*! warning: not thread safe */
char *strpos(coord_t *pos)
{
    snprintf(_strposbuf, ARRAY_DIM(_strposbuf), "[%lux%lu]", pos->_x % 10000, pos->_y % 10000);
    return _strposbuf;
}