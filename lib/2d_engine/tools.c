#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>
#include <stdarg.h>
#include "aoc_types.h"

int engine_draw_box(struct engine_t *_eng)
{
    int ret = 0;

    coord_t _topLeft = {._x = 0, ._y = 0};
    coord_t _topRight = {._x = (_eng->_boundaries._x) * _eng->_cellDim._x + 2, ._y = 0};
    coord_t _botRight = {._x = (_eng->_boundaries._x) * _eng->_cellDim._x + 2, ._y = (_eng->_boundaries._y) * _eng->_cellDim._y + 2};
    coord_t _botLeft = {._x = 0, ._y = (_eng->_boundaries._y) * _eng->_cellDim._y + 2};

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

int engine_fill_drawing_area(struct engine_t *_eng)
{
    if (_eng->_mustDraw)
    {
        for (size_t _line = 2; _line < (_eng->_boundaries._y + 1) * _eng->_cellDim._y + 1; _line++)
        {
            printf(MCUR_FMT, _line, (size_t)2);
            for (size_t _col = 0; _col < (_eng->_boundaries._x) * _eng->_cellDim._x; _col++)
            {
                printf("%c", _eng->_void_sym);
            }
        }
        engine_cursor_exit_drawing_area(_eng);
    }
}

int engine_fill_hv_line(struct engine_t *_eng, coord_t *_start, coord_t *_end, AOC_2D_DIR _dir, const char *_c)
{
    if (_eng->_mustDraw)
    {
        if (!_eng || !_start || _dir >= AOC_DIR_MAX)
            return EINVAL;
        printf(MCUR_FMT "%s", _start->_y, _start->_x, _c);
        put_pos(_eng, &_eng->_cursor, _start);
        do
            printf(MCUR_FMT "%s", _eng->_cursor._y, _eng->_cursor._x, _c);
        while (!move_cursor_until(_eng, _dir, 1, _end));
        engine_cursor_exit_drawing_area(_eng);
    }
    return 0;
}

void aoc_engine_list_objects(aoc_2d_engine_t _eng)
{
    if (_eng->_mustDraw)
    {
        engine_cursor_user_stats(_eng);
        LL_FOREACH(_node, _eng->_objects)
        {
            struct object_t *_obj = CAST(struct object_t *, _node);
            aoc_info("%s %ld:%ld", _obj->_name, _obj->_pos._x, _obj->_pos._y);
            engine_cursor_user_next_stats(_eng);
        }
    }
}

void aoc_engine_prompt_stats(aoc_2d_engine_t _eng)
{
    engine_cursor_stats(_eng);
    aoc_info("objects %ld", _eng->_objects._size);
    engine_cursor_private_next_stats(_eng);
    aoc_info("boundaries [%ldx%ld]", _eng->_boundaries._x, _eng->_boundaries._y);
    engine_cursor_private_next_stats(_eng);
}

void aoc_engine_prompt(aoc_2d_engine_t _eng, const size_t _sleep, size_t _count, ...)
{
    if (_eng->_mustDraw)
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