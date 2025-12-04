#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>

const char defaultName[] = "no name";
char _strobjcountbuf[] = LL_MAX_LEN_STR;

/* creates a part that will be attached to an object.
a part has :
-   coordinates
-   can be used for collision detection when moving the object
-   its own symbol to represent the part itself
-   a format (color/underline/bold...)
*/

part_h eng_part_create(struct object *obj, coord_t *sympos, char sym, char *fmt)
{
    part_h ret = malloc(sizeof(struct part));
    if (!ret)
        return NULL;
    memset(ret, 0, sizeof(struct part));
    if (fmt)
    {
        ret->_fmt = malloc(strnlen(fmt, ABSOLUTE_MAX_PART_FMT_LEN));
        if (!ret->_fmt)
            goto error;
    }
    ret->_pos._x = sympos->_x;
    ret->_pos._y = sympos->_y;
    ret->_sym = sym;
    return ret;
error:
    FREE(ret);
    return NULL;
}

void aoc_engine_free_part(void *arg)
{
    part_h _parth = (part_h)arg;
    if (_parth->_fmt)
        FREE(_parth->_fmt);
    FREE(_parth);
}

aoc_2d_object_h aoc_engine_object(aoc_2d_engine_h eng, const char *const name, coord_t *pos, char *sym, size_t props)
{
    struct object *_ret = malloc(sizeof(struct object));
    if (!_ret)
        return NULL;

    size_t _nameLen = strnlen(name, MAX_NAME_LEN_LUI) + 1;
    _ret->_name = malloc(_nameLen);
    if (!_ret->_name)
        goto free_rt;

    strncpy(_ret->_name, name, _nameLen);

    char _buf[ABSOLUTE_MAX_PART_CNT] = {0};
    char *_pc;

    strncpy(_buf, sym, ABSOLUTE_MAX_PART_CNT - 1);
    dll_head_init(&_ret->_parts);

    _pc = strtok(_buf, "\n");

    coord_t _rpos = {._x = pos->_x, ._y = pos->_y};

    assert(COORD_RANGE_CHECK(_rpos, _coordboundaries));

    while (_pc)
    {
        size_t _parti = 0;
        size_t _partcnt = 0;
        _partcnt = strnlen(_pc, ABSOLUTE_MAX_PART_CNT);
        while (_parti < _partcnt)
        {
            if (_buf[_parti] != ' ')
            {
                part_h _pns = eng_part_create(_ret, &_rpos, _buf[_parti], NULL);
                if (dll_node_append(&_ret->_parts, NODE_CAST(_pns)))
                    goto free_ll;
            }
            _rpos._x += 1;
            _parti++;
        }
        _rpos._y += 1;
        _rpos._x = 0;
        _pc = strtok(NULL, "\n");
    }
    _ret->_props = props;

    if (pos)
    {
        _ret->_pos._x = pos->_x;
        _ret->_pos._y = pos->_y;
    }
    else
    {
        //! should have a collision test.
        _ret->_pos._x = eng->_coordlimits._min._x;
        _ret->_pos._y = eng->_coordlimits._min._y;
    }

    aoc_engine_calculate_object_position(_ret);

    return _ret;

free_ll:
    dll_free_all(&_ret->_parts, aoc_engine_free_part);
free_name:
    FREE(_ret->_name);
free_rt:
    FREE(_ret);
    return NULL;
}

void aoc_engine_free_object(void *_data)
{
    aoc_2d_object_h _obj = (aoc_2d_object_h)_data;
    assert(0 == _obj->_refcnt && "trying to free an object with potential dangling reference");
    FREE(_obj->_name);
    dll_free_all(&_obj->_parts, aoc_engine_free_part);
    FREE(_obj);
}

int aoc_engine_draw_part(struct ascii_2d_engine *eng, part_h part, char *specfmt)
{
    int ret = 0;
    if (eng->_enabledraw)
    {
        printf(MCUR_FMT "%s", part->_pos._y + eng->_partoffset._y, part->_pos._x + eng->_partoffset._x, specfmt ? specfmt : "");
        printf("%s", part->_fmt ? part->_fmt : "");
        printf("%s", specfmt ? specfmt : "");
        printf("%c" RESET, part->_sym);
    }
    return ret;
}

int aoc_engine_draw_object(struct ascii_2d_engine *eng, struct object *obj, char *specfmt)
{
    int ret = 0;

    if (eng->_enabledraw)
    {
        dll_node_h _partdllnode;
        part_h _parth = NULL;
        LL_FOREACH_EXT(_partdllnode, obj->_parts)
        {
            _parth = CAST(part_h, _partdllnode);
            aoc_engine_draw_part(eng, _parth, specfmt);
        }
        usleep(1000 * eng->_delay);
    }
    return ret;
}

int aoc_engine_erase_object(struct ascii_2d_engine *eng, struct object *obj)
{
    int _ret = 0;

    if (eng->_enabledraw)
    {
        dll_node_h _part_node;
        part_h _sym;
        LL_FOREACH_EXT(_part_node, obj->_parts)
        {
            _sym = CAST(part_h, _part_node);
            printf(MCUR_FMT "%c", _sym->_pos._y + eng->_partoffset._y, _sym->_pos._x + eng->_partoffset._x, eng->_voidsym);
        }
        engine_cursor_exit_drawing_area(eng);
    }
    return _ret;
}

aoc_2d_object_h aoc_engine_get_obj_my_name(aoc_2d_engine_h _eng, const char *const name)
{
    LL_FOREACH(_node, _eng->_objects)
    {
        if (!strcmp(name, CAST(aoc_2d_object_h, _node)->_name))
            return CAST(aoc_2d_object_h, _node);
    }
    return NULL;
}

const char *const aoc_engine_get_obj_name(const aoc_2d_object_h const _obj)
{
    if (!_obj)
        return defaultName;
    return _obj->_name;
}

int aoc_engine_move_each_part(aoc_2d_engine_h eng, aoc_2d_object_h obj, size_t steps, AOC_2D_DIR dir)
{
    LL_FOREACH(_partnode, obj->_parts)
    {
        part_h _part = (part_h)_partnode;
        coord_t _prevpos = {._x = _part->_pos._x, ._y = _part->_pos._y};

        move_within_coord(eng, &_part->_pos, steps, dir);

        part_h other = aoc_engine_get_part_by_position(eng, &_prevpos);
        if (other)
            aoc_engine_draw_part(eng, other, NULL);
    }
    return 0;
}

int aoc_engine_put_object_and_redraw(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, coord_t _npos)
{
    if (!_eng || !_obj)
        return EINVAL;

    int ret = is_position_in_box(_eng, &_npos);
    if (ret)
        return ret;

    ret = aoc_engine_collision_at(_eng, _obj, &_npos);
    if (ret)
        return ret;

    aoc_engine_erase_object(_eng, _obj);
    put_pos(_eng, &_obj->_pos, &_npos);
    aoc_engine_draw_object(_eng, _obj, NULL);
    engine_put_cursor_in_footer_area(_eng);
    return 0;
}

part_h aoc_engine_get_part_by_position(aoc_2d_engine_h eng, coord_t *pos)
{
    LL_FOREACH(_objnode, eng->_objects)
    {
        aoc_2d_object_h _obj = (aoc_2d_object_h)_objnode;
        LL_FOREACH(_partnode, _obj->_parts)
        {
            part_h _part = (part_h)_partnode;
            if (pos->_x == _part->_pos._x && pos->_x == _part->_pos._x)
                return _part;
        }
    }
    return NULL;
}

int aoc_engine_check_move_possible(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t steps, AOC_2D_DIR dir)
{
    int ret = 0;
    LL_FOREACH(_partnode, _obj->_parts)
    {
        part_h _part = (part_h)_partnode;
        coord_t _testpos = {._x = _part->_pos._x, ._y = _part->_pos._y};
        ret = move_within_coord(_eng, &_testpos, steps, dir);
        if (ret)
        {
            aoc_engine_prompt_multistr(_eng, 0, _obj->_name, ": out of box at ", strpos(&_testpos));
            return ret;
        }
        ret = aoc_engine_collision_at(_eng, _obj, &_testpos);
        if (ret)
        {
            aoc_engine_prompt_multistr(_eng, 0, _obj->_name, ": collision with other object at ", strpos(&_testpos));
            return ret;
        }
    }
}

void aoc_engine_disable_collisions(aoc_2d_engine_h _eng)
{
    _eng->_enablecollisions = false;
}

void aoc_engine_enable_collisions(aoc_2d_engine_h _eng)
{
    _eng->_enablecollisions = true;
}

int aoc_engine_step_object(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t steps, AOC_2D_DIR dir, char *fmt)
{
    int _ret = 0;
    if (_eng->_enablecollisions)
        _ret = aoc_engine_check_move_possible(_eng, _obj, steps, dir);
    if (_ret)
        return _ret;
    aoc_engine_erase_object(_eng, _obj);

    _ret = aoc_engine_move_each_part(_eng, _obj, steps, dir);
    if (_ret)
    {
        engine_put_cursor_in_footer_area(_eng);
        aoc_err("aoc_engine_move_each_part:%s", strerror(_ret));
    }
    _ret = move_within_coord(_eng, &_obj->_pos, steps, dir);
    if (_ret)
    {
        engine_put_cursor_in_footer_area(_eng);
        aoc_err("move_within_coord:%s", strerror(_ret));
    }
    _ret = aoc_engine_draw_object(_eng, _obj, fmt);
    if (_ret)
    {
        engine_put_cursor_in_footer_area(_eng);
        aoc_err("aoc_engine_draw_object:%s", strerror(_ret));
    }
    engine_put_cursor_in_footer_area(_eng);
    return _ret;
}

int aoc_engine_calculate_object_position(aoc_2d_object_h obj)
{
    if (!obj)
        return EINVAL;
    coord_t _averagecoord = {0};
    LL_FOREACH(_partnode, obj->_parts)
    {
        part_h _part = (part_h)_partnode;
        _averagecoord._x += _part->_pos._x;
        _averagecoord._y += _part->_pos._y;
    }
    obj->_pos._x = _averagecoord._x / obj->_parts._size;
    obj->_pos._y = _averagecoord._y / obj->_parts._size;
    return 0;
}

coord_t aoc_engine_get_object_position(aoc_2d_engine_h _eng, aoc_2d_object_h _obj)
{
    coord_t _pos = {._x = 0, ._y = 0};
    assert(_eng && _obj && "NULL pointers");
    _pos._x = _obj->_pos._x;
    _pos._y = _obj->_pos._y;
    return _pos;
}

int aoc_engine_move_one_step_towards(aoc_2d_engine_h _eng, aoc_2d_object_h _a, coord_t _pos)
{
    int ret = 0;
    coord_t current = aoc_engine_get_object_position(_eng, _a);
    if (current._x < _pos._x)
        ret = aoc_engine_step_object(_eng, _a, 1LU, AOC_DIR_RIGHT, NULL);
    if (current._x > _pos._x && !ret)
        ret = aoc_engine_step_object(_eng, _a, 1LU, AOC_DIR_LEFT, NULL);
    if (current._y < _pos._y && !ret)
        ret = aoc_engine_step_object(_eng, _a, 1LU, AOC_DIR_DOWN, NULL);
    if (current._y > _pos._y && !ret)
        ret = aoc_engine_step_object(_eng, _a, 1LU, AOC_DIR_UP, NULL);
    return ret;
}

aoc_2d_object_ref_t *aoc_2d_object_ref(aoc_2d_object_h _obj)
{
    aoc_2d_object_ref_t *_ntracker = malloc(sizeof(aoc_2d_object_ref_t));
    if (!_ntracker)
        return NULL;
    _ntracker->_node._prev = NULL;
    _ntracker->_node._next = NULL;
    _ntracker->_blocked = false;
    _ntracker->data = _obj;
    _obj->_refcnt++;
    return _ntracker;
}

void aoc_2d_object_ref_free(void *arg)
{
    aoc_2d_object_ref_t *_ntracker = CAST(aoc_2d_object_ref_t *, arg);
    _ntracker->data->_refcnt--;
    FREE(_ntracker);
}

size_t aoc_engine_get_object_count(aoc_2d_engine_h _eng)
{
    return _eng->_objects._size % LL_MAX_LEN_LUI;
}

char *strobjcnt(aoc_2d_engine_h _eng)
{
    snprintf(_strobjcountbuf, ARRAY_DIM(_strobjcountbuf), "%lu", aoc_engine_get_object_count(_eng));
    return _strobjcountbuf;
}