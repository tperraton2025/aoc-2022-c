#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>

const char defaultName[] = "no name";

struct symbol_t *eng_sym_create(struct object *_obj, coord_t *_rpos, char _c)
{
    struct symbol_t *ret = malloc(sizeof(struct symbol_t));
    if (!ret)
        return NULL;
    ret->_pos._x = _rpos->_x;
    ret->_pos._y = _rpos->_y;
    ret->_sym = _c;
    return ret;
}

void eng_free_sym(void *_data)
{
    FREE(_data);
}

aoc_2d_object_h eng_obj_create(aoc_2d_engine_h _eng, const char *const _name, coord_t *_pos, char *_sym, size_t _props)
{
    struct object *ret = malloc(sizeof(struct object));
    if (!ret)
        return NULL;

    size_t _nameLen = strnlen(_name, ABSOLUTE_MAX_NAME_LEN) + 1;
    ret->_name = malloc(_nameLen);
    if (!ret->_name)
        goto free_rt;

    strncpy(ret->_name, _name, _nameLen);

    char _buf[ABSOLUTE_MAX_SYM_CNT] = "";
    char *_pc;

    strncpy(_buf, _sym, ABSOLUTE_MAX_SYM_CNT - 1);
    dll_head_init(&ret->_symbols);

    _pc = strtok(_buf, "\n");

    coord_t _rpos = {._x = 0, ._y = 0};
    while (_pc)
    {
        size_t _ii = 0;
        size_t _len = 0;
        _len = strnlen(_buf, ABSOLUTE_MAX_SYM_CNT);
        while (_ii < _len)
        {
            if (_buf[_ii] != ' ')
            {
                struct symbol_t *_pns = eng_sym_create(ret, &_rpos, _buf[_ii]);
                if (dll_node_append(&ret->_symbols, NODE_CAST(_pns)))
                    goto free_ll;
            }
            _rpos._x += 1;
            _ii++;
        }
        _rpos._y += 1;
        _rpos._x = 0;
        _pc = strtok(NULL, "\n");
    }
    ret->_props = _props;

    if (_pos)
    {
        ret->_pos._x = _pos->_x;
        ret->_pos._y = _pos->_y;
    }
    else
    {
        //! should have a collisiont test.
        ret->_pos._x = _eng->_boundaries._x - 1;
        ret->_pos._y = _eng->_boundaries._y - 1;
    }

    return ret;

free_ll:
    dll_free_all(&ret->_symbols, eng_free_sym);
free_name:
    FREE(ret->_name);
free_rt:
    FREE(ret);
    return NULL;
}

void eng_obj_free(void *_data)
{
    aoc_2d_object_h _obj = (aoc_2d_object_h)_data;
    FREE(_obj->_name);
    dll_free_all(&_obj->_symbols, eng_free_sym);
    FREE(_obj);
}

int draw_object(struct ascii_2d_engine *_eng, struct object *_obj, char *_fmt)
{
    int ret = 0;

    if (_eng->_mustDraw)
    {
        struct dll_node *_sym_node;
        struct symbol_t *_sym;
        LL_FOREACH_EXT(_sym_node, _obj->_symbols)
        {
            _sym = CAST(struct symbol_t *, _sym_node);
            coord_t _sympos = {._x = 0, ._y = 0};
            _sympos._y = ((_obj->_pos._y - 1) * _eng->_cellDim._y) + _sym->_pos._y + 2;
            _sympos._x = ((_obj->_pos._x - 1) * _eng->_cellDim._x) + _sym->_pos._x + 2;
            printf(MCUR_FMT "%s%c" RESET, _sympos._y, _sympos._x, _fmt ? _fmt : "", _sym->_sym);
        }
        usleep(1000 * _eng->_delay);
    }
    return ret;
}

int erase_object(struct ascii_2d_engine *_eng, struct object *_obj)
{
    int ret = 0;

    if (_eng->_mustDraw)
    {
        struct dll_node *_sym_node;
        struct symbol_t *_sym;
        LL_FOREACH_EXT(_sym_node, _obj->_symbols)
        {
            _sym = CAST(struct symbol_t *, _sym_node);
            coord_t _sympos = {._x = 0, ._y = 0};
            _sympos._y = (_obj->_pos._y - 1) * _eng->_cellDim._y + _sym->_pos._y + 2;
            _sympos._x = (_obj->_pos._x - 1) * _eng->_cellDim._x + _sym->_pos._x + 2;
            printf(MCUR_FMT "%c", _sympos._y, _sympos._x, _eng->_void_sym);
        }
        engine_cursor_exit_drawing_area(_eng);
    }
    return ret;
}

aoc_2d_object_h aoc_engine_get_obj_my_name(aoc_2d_engine_h _eng, const char *name)
{
    LL_FOREACH(_node, _eng->_objects)
    {
        if (!strcmp(name, CAST(aoc_2d_object_h, _node)->_name))
            return CAST(aoc_2d_object_h, _node);
    }
    return NULL;
}

const char *const aoc_engine_get_obj_name(aoc_2d_object_h _obj)
{
    if (!_obj)
        return defaultName;
    return _obj->_name;
}
