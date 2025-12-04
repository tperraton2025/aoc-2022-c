#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>
#include <stdarg.h>
#include "aoc_types.h"

void eng_set_refresh_delay(aoc_2d_engine_h _eng, size_t delay)
{
    _eng->_delay = delay;
}

int engine_draw_box(struct ascii_2d_engine *_eng)
{
    int ret = 0;

    coord_t _topLeft = {._x = _eng->_drawlimits._min._x, ._y = _eng->_drawlimits._min._y};
    coord_t _topRight = {._x = _eng->_drawlimits._max._x, ._y = _eng->_drawlimits._min._y};
    coord_t _botRight = {._x = _eng->_drawlimits._max._x, ._y = _eng->_drawlimits._max._y};
    coord_t _botLeft = {._x = _eng->_drawlimits._min._x, ._y = _eng->_drawlimits._max._y};

    engine_fill_hv_line(_eng, &_topLeft, &_topRight, AOC_DIR_RIGHT, "═");
    engine_fill_hv_line(_eng, &_topRight, &_botRight, AOC_DIR_DOWN, "║");
    engine_fill_hv_line(_eng, &_botRight, &_botLeft, AOC_DIR_LEFT, "═");
    engine_fill_hv_line(_eng, &_botLeft, &_topLeft, AOC_DIR_UP, "║");

    ret = engine_draw_symbol_at(_eng, &_topLeft, "╔");
    if (ret)
        goto error;
    ret = engine_draw_symbol_at(_eng, &_topRight, "╗");
    if (ret)
        goto error;
    ret = engine_draw_symbol_at(_eng, &_botRight, "╝");
    if (ret)
        goto error;
    ret = engine_draw_symbol_at(_eng, &_botLeft, "╚");
    if (ret)
        goto error;

    put_pos(_eng, &_eng->_cursor, &_topLeft);
error:
    return ret;
}

int engine_fill_drawing_area(struct ascii_2d_engine *_eng)
{
    if (_eng->_enabledraw)
    {
        for (size_t _line = _eng->_coordlimits._min._y; _line <= _eng->_coordlimits._max._y; _line++)
        {
            for (size_t _col = _eng->_coordlimits._min._x; _col <= _eng->_coordlimits._max._x; _col++)
            {
                coord_t _coord = {._x = _col, ._y = _line};
                engine_draw_part_at(_eng, &_coord, &_eng->_voidsym);
            }
        }
    }
}

int engine_fill_hv_line(struct ascii_2d_engine *_eng, coord_t *_start, coord_t *_end, AOC_2D_DIR _dir, const char *_c)
{
    if (_eng->_enabledraw)
    {
        if (!_eng || !_start || _dir >= AOC_DIR_MAX)
            return EINVAL;
        engine_draw_symbol_at(_eng, _start, _c);
        put_pos(_eng, &_eng->_cursor, _start);
        do
            engine_draw_symbol_at(_eng, &_eng->_cursor, _c);

        while (!move_cursor_until(_eng, _dir, 1, _end));
        engine_cursor_exit_drawing_area(_eng);
    }
    return 0;
}

void aoc_engine_list_objects(aoc_2d_engine_h _eng)
{
    if (_eng->_enabledraw)
    {
        engine_cursor_user_stats(_eng);
        LL_FOREACH(_node, _eng->_objects)
        {
            struct object *_obj = CAST(struct object *, _node);
            coord_t _pos = aoc_engine_get_object_position(_eng, _obj);
            aoc_info("%s %ld:%ld", _obj->_name, _pos._x, _pos._y);
            engine_cursor_user_next_stats(_eng);
        }
        engine_put_cursor_in_footer_area(_eng);
    }
}

void aoc_engine_prompt_stats(aoc_2d_engine_h _eng)
{
    engine_cursor_stats(_eng);
    aoc_info("objects %ld", _eng->_objects._size);
    engine_cursor_private_next_stats(_eng);
    aoc_info("box size %s", strpos(&_eng->_coordlimits._max)); 
    engine_cursor_private_next_stats(_eng);
}

void aoc_engine_prompt(aoc_2d_engine_h _eng, const size_t _sleep, size_t _count, ...)
{
    if (_eng->_enabledraw)
    {
        engine_put_cursor_in_footer_area(_eng);
        printf(ERASE_LINE_FROM_CR);

        va_list _ap;
        va_start(_ap, _count);
        for (size_t _ii = 0; _ii < _count; _ii++)
        {
            char *_extra_str = va_arg(_ap, char *);
            printf("%s ", _extra_str);
        }
        va_end(_ap);

        if (_sleep)
            usleep(_sleep * 1000);
    }
}