#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>
#include <math.h>
#include <fenv.h>
#include <fcntl.h>

void engine_free(aoc_2d_engine_h _eng)
{
    engine_cursor_exit_drawing_area(_eng);
    dll_free_all(&_eng->_objects, aoc_engine_free_object);
    FREE(_eng);
}

DLL_NODE_CTOR(coord_tracker_t, coordtracker_ctor);

aoc_2d_engine_h engine_create(coord_t *ncoordlimits, char _voidsym, size_t delay)
{
    aoc_2d_engine_h _eng = NULL;
    TRY_RAII_MALLOC(_eng, sizeof(struct ascii_2d_engine));

    if (!_eng)
        return NULL;

    dll_head_init(&_eng->_objects);

    _eng->_delay = delay;

    if (COORD_RANGE_CHECK_P(ncoordlimits, _drawlimits))
    {
        _eng->_partoffset._x = 2;
        _eng->_partoffset._y = 2;

        _eng->_drawlimits._min._x = 1;
        _eng->_drawlimits._min._y = 1;

        _eng->_drawlimits._max._x = ncoordlimits->_x + _eng->_partoffset._x + 1;
        _eng->_drawlimits._max._y = ncoordlimits->_y + _eng->_partoffset._y + 1;

        _eng->_coordlimits._min._x = 0;
        _eng->_coordlimits._min._y = 0;

        _eng->_coordlimits._max._x = ncoordlimits->_x;
        _eng->_coordlimits._max._y = ncoordlimits->_y;

        _eng->_cursor._x = _eng->_coordlimits._min._x;
        _eng->_cursor._y = _eng->_coordlimits._min._y;

        _eng->_voidsym = _voidsym;

        _eng->_statLine = 2;
        _eng->_PrivStatLine = _eng->_statLine;

        _eng->_enablecollisions = true;

        _eng->_newlinecnt = _eng->_coordlimits._min._y;

        _eng->_enabledraw = _eng->_drawlimits._max._x < DRAWABLE_MAX_X && _eng->_drawlimits._max._y < DRAWABLE_MAX_Y;
        if (!_eng->_enabledraw)
            aoc_warn("%s", "drawable zone will not fit in a terminal window \n\
                drawing is disabled until forced");
        return _eng;
    }
    else
        goto cleanup;

cleanup:
    FREE(_eng);
    return NULL;
}

int engine_extend_drawing_area(struct ascii_2d_engine *_eng, coord_t newlimits)
{
    if (!_eng)
        return EINVAL;

    if (!COORD_RANGE_CHECK(newlimits, _drawlimits))
        return ERANGE;
    else
    {
        _eng->_drawlimits._max._x = newlimits._x + _eng->_partoffset._x + 1;
        _eng->_drawlimits._max._y = newlimits._y + _eng->_partoffset._y + 1;

        _eng->_coordlimits._max._x = newlimits._x;
        _eng->_coordlimits._max._y = newlimits._y;
        return 0;
    }
    return EINVAL;
}

int engine_draw_part_at(aoc_2d_engine_h eng, coord_t *_pos, char *_sym)
{
    if (eng->_enabledraw)
    {
        coord_t _pcord = {._x = _pos->_x + eng->_partoffset._x, ._y = _pos->_y + eng->_partoffset._y};
        engine_draw_symbol_at(eng, &_pcord, _sym);
    }
    return 0;
}

int engine_draw_symbol_at(aoc_2d_engine_h eng, coord_t *_pos, const char *_sym)
{
    if (eng->_enabledraw)
    {
        printf(MCUR_FMT "%s", _pos->_y, _pos->_x, _sym);
        engine_cursor_exit_drawing_area(eng);
    }
    return 0;
}

int engine_draw(struct ascii_2d_engine *_eng)
{
    printf(ERASE_DISPLAY);
    printf(HOME);
    if (_eng->_enabledraw)
    {
        assert(COORD_RANGE_CHECK(_eng->_drawlimits._max, _drawlimits));
        assert(COORD_RANGE_CHECK(_eng->_drawlimits._min, _drawlimits));

        assert(COORD_RANGE_CHECK(_eng->_coordlimits._max, _coordboundaries));
        assert(COORD_RANGE_CHECK(_eng->_coordlimits._min, _coordboundaries));

        engine_draw_box(_eng);
        engine_fill_drawing_area(_eng);
        coord_t _pos = {0};
        aoc_engine_prompt_stats(_eng);
        aoc_engine_prompt_obj_list(_eng);
        engine_draw_objects(_eng, &_pos);
        engine_put_cursor_in_footer_area(_eng);
    }
    return 0;
}

int engine_draw_objects(aoc_2d_engine_h _eng, coord_t *_corner)
{
    if (_eng->_enabledraw)
    {
        printf("\n");
        LL_FOREACH(_obj_node, _eng->_objects)
        {
            struct object *_obj = CAST(struct object *, _obj_node);
            aoc_engine_draw_object(_eng, _obj, NULL);
        }
        printf("\n");
        usleep(1000 * _eng->_delay);
    }
    return 0;
}

int aoc_engine_append_obj(aoc_2d_engine_h engine, aoc_2d_object_h newobj)
{
    coord_t _extension = {0};

    LL_FOREACH(_partiter, newobj->_parts)
    {
        part_h _part = (part_h)_partiter;
        _extension._x = HIGHEST(engine->_coordlimits._max._x, _part->_pos._x);
        _extension._y = HIGHEST(engine->_coordlimits._max._y, _part->_pos._y);
    }

    if (!COORD_RANGE_CHECK(_extension, _drawlimits))
        return ERANGE;

    int ret = dll_node_append(&engine->_objects, CAST(dll_node_h, newobj));
    if (ret)
    {
        aoc_err("Failed to append %s at %s: %s", newobj->_name, strpos(&newobj->_pos), strerror(ret));
        return EINVAL;
    }

    if (_extension._x > engine->_coordlimits._max._x || _extension._y > engine->_coordlimits._max._y)
    {
        ret = engine_extend_drawing_area(engine, _extension);
        if (ret)
        {
            /* user has responsibility to free the unusable object */
            dll_node_disconnect(&engine->_objects, CAST(dll_node_h, newobj));
            aoc_err("Failed to append %s at %s: %s", newobj->_name, strpos(&newobj->_pos), strerror(ret));
            return ERANGE;
        }
        aoc_info("box extended %s for %s", strpos(&engine->_coordlimits._max), newobj->_name);
    }
    return 0;
}

/* will implement 2 methods, one by map and this one by objects linked list */
aoc_2d_object_h aoc_engine_get_obj_by_position(aoc_2d_engine_h _eng, coord_t *_pos)
{
    LL_FOREACH(_objNode, _eng->_objects)
    {
        aoc_2d_object_h _object = CAST(aoc_2d_object_h, _objNode);
        if (_object->_pos._x == _pos->_x && _object->_pos._y == _pos->_y)
        {
            return _object;
        }

        // LL_FOREACH(_partnode, _object->_parts)
        //{
        //     part_h part = (part_h)_partnode;
        //     coord_t *_partpos = &part->_pos;
        //     if (_partpos->_x == _pos->_x && _partpos->_y == _pos->_y)
        //     {
        //         return _object;
        //     }
        // }
    }
    return NULL;
}

/* will implement 2 methods, one by map and this one by objects linked list */
struct dll_head *aoc_engine_prompt_obj_list_with_a_part_at_position(aoc_2d_engine_h _eng, coord_t *_pos)
{
    struct dll_head *_list;
    dll_head_init(_list);
    LL_FOREACH(_objNode, _eng->_objects)
    {
        aoc_2d_object_h _object = CAST(aoc_2d_object_h, _objNode);

        LL_FOREACH(_partnode, _object->_parts)
        {
            part_h part = (part_h)_partnode;
            coord_t *_partpos = &part->_pos;
            if (_partpos->_x == _pos->_x && _partpos->_y == _pos->_y)
            {
                aoc_2d_object_ref_h _ref = aoc_2d_object_ref(_object);
                dll_node_append(_list, &_ref->_node);
            }
        }
    }
    return _list;
}

int aoc_engine_object_fit_detect(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t steps, AOC_2D_DIR dir)
{
    int ret = 0;

    coord_t _test = {._x = _obj->_pos._x, ._y = _obj->_pos._y};
    if (move_within_window(_eng, &_test, steps, dir))
        return EINVAL;
    aoc_2d_object_h _other = aoc_engine_get_obj_by_position(_eng, &_test);
    if (!_other)
        return 0;
    if (_other->_props & OBJ_PROPERTY_NO_COLLISION)
        return 0;
    return 1;
}

int aoc_engine_collision_at(aoc_2d_engine_h _eng, aoc_2d_object_h excl, coord_t *_pos)
{
    if (!_eng)
        return EINVAL;
    aoc_2d_object_h _other = aoc_engine_get_obj_by_position(_eng, _pos);
    if (!_other || _other == excl)
        return 0;
    if (_other->_props & OBJ_PROPERTY_NO_COLLISION)
        return 0;
    return EALREADY;
}

int aoc_engine_adapt_objects_to_extension(aoc_2d_engine_h _eng, size_t steps, AOC_2D_DIR _dir)
{
    int _ret = 0;
    _eng->_enablecollisions = false;
    _eng->_enabledraw = false;
    LL_FOREACH(obj_node, _eng->_objects)
    {
        aoc_2d_object_h _objh = CAST(aoc_2d_object_h, obj_node);
        for (size_t _it = 0; _it < steps; _it++)
        {
            _ret = aoc_engine_step_object(_eng, _objh, 1, _dir, NULL);
            if (_ret)
            {
                engine_put_cursor_in_footer_area(_eng);
                aoc_err("aoc_engine_step_object:%s %s", _objh->_name, strerror(_ret));
            }
        }
    }
    _eng->_enabledraw = true;
    _eng->_enablecollisions = true;
}

int aoc_engine_extend_one_direction(aoc_2d_engine_h _eng, size_t steps, AOC_2D_DIR _dir)
{
    size_t delta = steps;
    if (!_eng || _dir >= AOC_DIR_MAX)
        return EINVAL;
    int _ret = 0;
    if (_dir > AOC_DIR_DOWN)
    {
        if ((_eng->_coordlimits._max._x + steps) > ABSOLUTE_MAX_X)
            return EINVAL;

        _eng->_coordlimits._max._x += steps;
        _eng->_drawlimits._max._x += steps;

        if (_dir == AOC_DIR_LEFT)
            aoc_engine_adapt_objects_to_extension(_eng, steps, AOC_DIR_RIGHT);
    }
    else
    {
        if ((_eng->_coordlimits._max._y + steps) > ABSOLUTE_MAX_Y)
            return EINVAL;

        _eng->_coordlimits._max._y += steps;
        _eng->_drawlimits._max._y += steps;

        if (_dir == AOC_DIR_UP)
            aoc_engine_adapt_objects_to_extension(_eng, delta, AOC_DIR_DOWN);
    }
    return 0;
}

coordboundaries_t aoc_engine_get_parts_boundaries(aoc_2d_engine_h _eng)
{
    coordboundaries_t ret = {0};
    memcpy(&ret, &_eng->_coordlimits, sizeof(coordboundaries_t));
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

void engine_clear_screen()
{
    printf(ERASE_DISPLAY);
}

void engine_activate_drawing(aoc_2d_engine_h _eng)
{
    _eng->_enabledraw = true;
}

void engine_deactivate_drawing(aoc_2d_engine_h _eng)
{
    _eng->_enabledraw = false;
}

void engine_add_line(aoc_2d_engine_h _eng)
{
    assert(_eng->_newlinecnt < ABSOLUTE_MAX_Y);
    _eng->_newlinecnt++;
}

size_t engine_last_line(aoc_2d_engine_h _eng)
{
    return _eng->_newlinecnt % ABSOLUTE_MAX_Y;
}

int aoc_engine_foreach_object_arg(aoc_2d_engine_h _eng, void *arg, void func(coord_t *pos, void *arg))
{
    LL_FOREACH(_it, _eng->_objects)
    {
        aoc_2d_object_h _objh = (aoc_2d_object_h)_it;
        func(&_objh->_pos, arg);
    }
}

dll_head_h engine_get_objects_positions(aoc_2d_engine_h _eng)
{

    dll_head_h _poslists = malloc(sizeof(dll_head_t));
    dll_head_init(_poslists);

    LL_FOREACH(_objn, _eng->_objects)
    {
        aoc_2d_object_h _objh = (aoc_2d_object_h)_objn;
        coord_tracker_h _ntr = coordtracker_ctor();
        _ntr->_coord._x = _objh->_pos._x;
        _ntr->_coord._y = _objh->_pos._y;
        dll_node_append(_poslists, &_ntr->_node);
    }
    return _poslists;
}

int engine_remove_object(aoc_2d_engine_h eng, aoc_2d_object_h obj)
{
    if (!eng || !obj)
        return EINVAL;
    aoc_engine_erase_object(eng, obj);
    dll_node_disconnect(&eng->_objects, &obj->_node);
    aoc_engine_free_object(&obj->_node);
    return 0;
}
