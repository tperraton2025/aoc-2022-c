#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>
#include <math.h>
#include <fenv.h>
#include <fcntl.h>

void engine_free(aoc_2d_engine_h _eng)
{
    engine_cursor_exit_drawing_area(_eng);
    dll_free_all(&_eng->_objects, eng_obj_free);
    FREE(_eng);
}

aoc_2d_engine_h engine_create(coord_t _res, coord_t _spce, char _void_sym)
{
    aoc_2d_engine_h _ret = malloc(sizeof(struct ascii_2d_engine));

    if (!_ret)
        return NULL;
    coord_t _boundary_MAX = {._x = ABSOLUTE_MAX_X, ._y = ABSOLUTE_MAX_Y};

    dll_head_init(&_ret->_objects);

    if (BOUNDARY_CHECK(_res, _boundary_MAX) && BOUNDARY_CHECK(_spce, _boundary_MAX))
    {
        _ret->_boundaries._x = 0;
        _ret->_boundaries._y = 0;

        _ret->_cursor._x = 0;
        _ret->_cursor._y = 0;

        _ret->_cellDim._x = _res._x;
        _ret->_cellDim._y = _res._y;

        _ret->_cellSpace._x = _spce._x;
        _ret->_cellSpace._y = _spce._y;

        _ret->_void_sym = _void_sym;
        _ret->_statLine = 2;
        _ret->_PrivStatLine = _ret->_statLine;

        _ret->_prompter = 0;
        _ret->_mustDraw = true;
        return _ret;
    }
    else
        goto cleanup;

cleanup:
    FREE(_ret);
    return NULL;
}

int engine_resize(struct ascii_2d_engine *_eng, coord_t _ns)
{
    if (!_eng)
        return EINVAL;

    coord_t _boundary_MAX = {._x = ABSOLUTE_MAX_X, ._y = ABSOLUTE_MAX_Y};
    if (BOUNDARY_CHECK(_ns, _eng->_boundaries))
        return 0;
    if (BOUNDARY_CHECK(_ns, _boundary_MAX))
    {
        _eng->_boundaries._x = _ns._x;
        _eng->_boundaries._y = _ns._y;
        return 0;
    }
    return EINVAL;
}

int engine_draw_symbol_at(aoc_2d_engine_h _eng, coord_t *_pos, char *_sym)
{
    if (_eng->_mustDraw)
    {
        printf(MCUR_FMT "%s", _pos->_y, _pos->_x, _sym);
        engine_cursor_exit_drawing_area(_eng);
    }
    return 0;
}

int engine_draw(struct ascii_2d_engine *_eng)
{
    if (_eng->_mustDraw)
    {
        int ret = 0;
        printf(ERASE_DISPLAY);
        engine_draw_box(_eng);
        engine_fill_drawing_area(_eng);
        coord_t _pos = {0};
        aoc_engine_prompt_stats(_eng);
        return engine_draw_objects(_eng, &_pos);
    }
}

int engine_draw_objects(aoc_2d_engine_h _eng, coord_t *_corner)
{
    int ret = 0;
    if (_eng->_mustDraw)
    {
        printf("\n");
        LL_FOREACH(_obj_node, _eng->_objects)
        {
            struct object *_obj = CAST(struct object *, _obj_node);
            draw_object(_eng, _obj, NULL);
        }
        printf("\n");
        usleep(1000 * _eng->_delay);
    }
    return ret;
}

int aoc_engine_append_obj(aoc_2d_engine_h _eng, aoc_2d_object_h _obj)
{
    const coord_t _boundary_MAX = {._x = ABSOLUTE_MAX_X, ._y = ABSOLUTE_MAX_Y};
    const coord_t _prev_bound = {._x = _eng->_boundaries._x, ._y = _eng->_boundaries._y};

    if (engine_resize(_eng, _obj->_pos))
        return EINVAL;

    if (dll_node_append(&_eng->_objects, CAST(struct dll_node *, _obj)))
    {
        aoc_err("Failed to append %s", _obj->_name);
        engine_resize(_eng, _prev_bound);
        return EINVAL;
    }
    else
        return 0;
}

aoc_2d_object_h aoc_engine_get_obj_by_position(aoc_2d_engine_h _eng, coord_t *_pos)
{
    LL_FOREACH(_objNode, _eng->_objects)
    {
        coord_t *_at = &CAST(aoc_2d_object_h, _objNode)->_pos;
        if (_at->_x == _pos->_x && _at->_y == _pos->_y)
        {
            return CAST(aoc_2d_object_h, _objNode);
        }
    }
    return NULL;
}

int aoc_engine_collision_detect(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t steps, AOC_2D_DIR dir)
{
    coord_t _test = {._x = _obj->_pos._x, ._y = _obj->_pos._y};
    if (move_pos(_eng, &_test, steps, dir))
        return EINVAL;
    aoc_2d_object_h _other = aoc_engine_get_obj_by_position(_eng, &_test);
    if (!_other)
        return 0;
    if (_other->_props & OBJ_PROPERTY_NO_COLLISION)
        return 0;
    return 1;
}

int aoc_engine_collision_at(aoc_2d_engine_h _eng, coord_t *_pos)
{
    if (!_eng)
        return EINVAL;
    aoc_2d_object_h _other = aoc_engine_get_obj_by_position(_eng, _pos);
    if (!_other)
        return 0;
    if (_other->_props & OBJ_PROPERTY_NO_COLLISION)
        return 0;
    return 1;
}

int aoc_engine_move_object(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t _steps, AOC_2D_DIR dir)
{
    if (!_eng || !_obj || dir >= AOC_DIR_MAX)
        return EINVAL;

    int ret = aoc_engine_collision_detect(_eng, _obj, _steps, dir);
    if (ret)
        return ret;

    move_pos(_eng, &_obj->_pos, _steps, dir);
    engine_put_cursor_in_footer_area(_eng);
}

int aoc_engine_put_object_and_redraw(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, coord_t _npos)
{
    if (!_eng || !_obj)
        return EINVAL;

    int ret = is_within_boundaries(_eng, &_npos);
    if (ret)
        return ret;

    ret = aoc_engine_collision_at(_eng, &_npos);
    if (ret)
        return ret;

    erase_object(_eng, _obj);
    put_pos(_eng, &_obj->_pos, &_npos);
    draw_object(_eng, _obj, NULL);
    engine_put_cursor_in_footer_area(_eng);
    return 0;
}

int aoc_engine_step_object_and_redraw(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, AOC_2D_DIR dir, char *fmt)
{
    int ret = 0;
    coord_t _prevPos = {._x = _obj->_pos._x, ._y = _obj->_pos._y};
    coord_t _test = {._x = _obj->_pos._x, ._y = _obj->_pos._y};
    if (move_pos(_eng, &_test, (size_t)1, dir))
        return EINVAL;

    ret = aoc_engine_collision_at(_eng, &_test);
    if (ret)
        return ret;

    erase_object(_eng, _obj);
    move_pos(_eng, &_obj->_pos, (size_t)1, dir);

    aoc_2d_object_h other = aoc_engine_get_obj_by_position(_eng, &_prevPos);

    other = aoc_engine_get_obj_by_position(_eng, &_prevPos);
    if (other)
        draw_object(_eng, other, NULL);
    draw_object(_eng, _obj, fmt);
    engine_put_cursor_in_footer_area(_eng);
    return ret;
}

int aoc_engine_move_object_and_redraw(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t steps, AOC_2D_DIR dir)
{
    int ret = aoc_engine_collision_detect(_eng, _obj, steps, dir);
    if (ret)
        return ret;

    if (!_eng || !_obj || dir >= AOC_DIR_MAX)
        return EINVAL;

    erase_object(_eng, _obj);
    struct dll_node *_sym_node;
    struct symbol_t *_sym;
    LL_FOREACH_EXT(_sym_node, _obj->_symbols)
    {
        _sym = CAST(struct symbol_t *, _sym_node);
        move_symbol(_eng, &_sym->_pos, steps, dir);
    }
    move_pos(_eng, &_obj->_pos, steps, dir);
    draw_object(_eng, _obj, NULL);
    engine_put_cursor_in_footer_area(_eng);
}

int aoc_engine_resize_one_direction(aoc_2d_engine_h _eng, size_t steps, AOC_2D_DIR _dir)
{
    if (!_eng || _dir >= AOC_DIR_MAX)
        return EINVAL;

    if (_dir > AOC_DIR_DOWN)
    {
        if ((_eng->_boundaries._x + steps) > ABSOLUTE_MAX_X)
            return EINVAL;
        _eng->_boundaries._x += steps;
    }
    else
    {
        if ((_eng->_boundaries._y + steps) > ABSOLUTE_MAX_Y)
            return EINVAL;
        _eng->_boundaries._y += steps;
    }
    struct dll_node *_sym_node;
    struct symbol_t *_sym;
    LL_FOREACH(obj_node, _eng->_objects)
    {
        aoc_engine_move_object(_eng, CAST(aoc_2d_object_h, obj_node), steps, _dir);
    }
    return 0;
}

coord_t aoc_engine_get_boundaries(aoc_2d_engine_h _eng)
{
    coord_t ret = {_eng->_boundaries._x, _eng->_boundaries._y};
    return ret;
}

coord_t aoc_engine_get_object_position(aoc_2d_engine_h _eng, aoc_2d_object_h _obj)
{
    coord_t _pos = {._x = 0, ._y = 0};
    assert(_eng && _obj && "NULL pointers");
    _pos._x = _obj->_pos._x;
    _pos._y = _obj->_pos._y;
    return _pos;
}

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

size_t aoc_engine_get_dist_between_objects(aoc_2d_engine_h _eng, aoc_2d_object_h _a, aoc_2d_object_h _b)
{
    coord_t _apos = aoc_engine_get_object_position(_eng, _a);
    coord_t _bpos = aoc_engine_get_object_position(_eng, _b);
    double _dX = HIGHEST(_apos._x, _bpos._x) - LOWEST(_apos._x, _bpos._x);
    double _dY = HIGHEST(_apos._y, _bpos._y) - LOWEST(_apos._y, _bpos._y);

    return (size_t)floor(sqrt((_dX * _dX) + (_dY * _dY)));
}

int aoc_engine_move_one_step_towards(aoc_2d_engine_h _eng, aoc_2d_object_h _a, coord_t _pos)
{
    int ret = 0;
    coord_t current = aoc_engine_get_object_position(_eng, _a);
    if (current._x < _pos._x)
        ret = aoc_engine_step_object_and_redraw(_eng, _a, AOC_DIR_RIGHT, NULL);
    if (current._x > _pos._x && !ret)
        ret = aoc_engine_step_object_and_redraw(_eng, _a, AOC_DIR_LEFT, NULL);
    if (current._y < _pos._y && !ret)
        ret = aoc_engine_step_object_and_redraw(_eng, _a, AOC_DIR_DOWN, NULL);
    if (current._y > _pos._y && !ret)
        ret = aoc_engine_step_object_and_redraw(_eng, _a, AOC_DIR_UP, NULL);
    return ret;
}

void engine_clear_screen()
{
    printf(ERASE_DISPLAY);
}

void engine_activate_drawing(aoc_2d_engine_h _eng)
{
    _eng->_mustDraw = true;
}

void engine_deactivate_drawing(aoc_2d_engine_h _eng)
{
    _eng->_mustDraw = false;
}