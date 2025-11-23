#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>

int move_cursor_until(struct engine_t *_eng, AOC_2D_DIR _dir, size_t _steps, coord_t *limit)
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

int move_symbol(aoc_2d_engine_t _eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir)
{
    switch (_dir)
    {
    case AOC_DIR_UP:
        if (_pos->_y < _eng->_cellDim._y * _steps)
            return ERANGE;
        _pos->_y -= _eng->_cellDim._y * _steps;
        break;
    case AOC_DIR_DOWN:
        _pos->_y += _eng->_cellDim._y * _steps;
        break;
    case AOC_DIR_LEFT:
        if (_pos->_x < _eng->_cellDim._x * _steps)
            return ERANGE;
        _pos->_x -= _eng->_cellDim._x * _steps;
        break;
    case AOC_DIR_RIGHT:
        _pos->_x += _eng->_cellDim._x * _steps;
        break;

    default:
        return EINVAL;
    }
    return 0;
}

int is_within_boundaries(aoc_2d_engine_t _eng, coord_t *_pos)
{
    if (!N_IN_RANGE(_pos->_y, 2, _eng->_boundaries._y))
        return ERANGE;
    if (!N_IN_RANGE(_pos->_x, 1, _eng->_boundaries._x))
        return ERANGE;
    return 0;
}

int move_pos(aoc_2d_engine_t _eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir)
{
    switch (_dir)
    {
    case AOC_DIR_UP:
        if (!N_IN_RANGE(_pos->_y - _steps, 2, _eng->_boundaries._y))
            return ERANGE;
        _pos->_y -= _steps;
        break;
    case AOC_DIR_DOWN:
        if (!N_IN_RANGE(_pos->_y + _steps, 1, _eng->_boundaries._y))
            return ERANGE;
        _pos->_y += _steps;
        break;
    case AOC_DIR_LEFT:
        if (!N_IN_RANGE(_pos->_x - _steps, 1, _eng->_boundaries._x))
            return ERANGE;
        _pos->_x -= _steps;
        break;
    case AOC_DIR_RIGHT:
        if (!N_IN_RANGE(_pos->_x + _steps, 1, _eng->_boundaries._x))
            return ERANGE;
        _pos->_x += _steps;
        break;

    default:
        return EINVAL;
    }
    return 0;
}

int put_pos(aoc_2d_engine_t _eng, coord_t *_pos, coord_t *_npos)
{
    int ret = is_within_boundaries(_eng, _npos);
    if (ret)
        return ret;

    _pos->_y = _npos->_y;
    _pos->_x = _npos->_x;

    return 0;
}

int put_symbol(aoc_2d_engine_t _eng, struct symbol_t *_sym, coord_t *_delta)
{
    if (!N_IN_RANGE(_delta->_y, 2, _eng->_boundaries._y))
        return ERANGE;
    if (!N_IN_RANGE(_delta->_x, 1, _eng->_boundaries._x))
        return ERANGE;
    return 0;
}

int engine_put_cursor_in_footer_area(struct engine_t *_eng)
{
    if (!_eng)
        return EINVAL;
    _eng->_prompter = 0;
    printf(MCUR_FMT ERASE_LINE_FROM_CR, (_eng->_boundaries._y * _eng->_cellDim._y) + 3, (size_t)0);
    return 0;
}

int engine_cursor_log(struct engine_t *_eng)
{
    if (!_eng)
        return EINVAL;
    printf(MCUR_FMT ERASE_LINE_FROM_CR, (_eng->_boundaries._y * _eng->_cellDim._y), (size_t)0);
    return 0;
}

int engine_cursor_stats(struct engine_t *_eng)
{
    if (!_eng)
        return EINVAL;
    _eng->_statLine = 2;
    if (_eng->_mustDraw)
        printf(MCUR_FMT ERASE_LINE_FROM_CR, _eng->_statLine++, ((_eng->_boundaries._x + 3) * _eng->_cellDim._x));
    return 0;
}

int engine_cursor_user_stats(struct engine_t *_eng)
{
    if (!_eng)
        return EINVAL;
    _eng->_statLine = _eng->_PrivStatLine;
    if (_eng->_mustDraw)
        printf(MCUR_FMT ERASE_LINE_FROM_CR, _eng->_statLine++, ((_eng->_boundaries._x + 3) * _eng->_cellDim._x));
    return 0;
}

int engine_cursor_private_next_stats(struct engine_t *_eng)
{
    if (!_eng)
        return EINVAL;
    if (_eng->_mustDraw)
        printf(MCUR_FMT ERASE_LINE_FROM_CR, _eng->_statLine++, ((_eng->_boundaries._x + 3) * _eng->_cellDim._x));
    _eng->_PrivStatLine = _eng->_statLine + 1;
    return 0;
}

int engine_cursor_user_next_stats(struct engine_t *_eng)
{
    if (!_eng)
        return EINVAL;
    if (_eng->_mustDraw)
        printf(MCUR_FMT ERASE_LINE_FROM_CR, _eng->_statLine++, ((_eng->_boundaries._x + 3) * _eng->_cellDim._x));
    else
        printf(MCUR_FMT ERASE_LINE_FROM_CR, (size_t)4, (size_t)2);

    if (_eng->_statLine >= _eng->_boundaries._y * _eng->_cellDim._y)
        _eng->_statLine = _eng->_PrivStatLine + 1;
    return 0;
}

int engine_cursor_exit_drawing_area(struct engine_t *_eng)
{
    if (!_eng)
        return EINVAL;
    printf(MCUR_FMT ERASE_LINE_FROM_CR, _eng->_boundaries._y * _eng->_cellDim._y + 10, (size_t)0);
    return 0;
}