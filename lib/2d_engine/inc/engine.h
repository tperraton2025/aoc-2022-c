#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ansi_cmd.h>
#include <aoc_linked_list.h>
#include "aoc_2d_engine.h"
#include "aoc_ranges.h"
#include "aoc_types.h"

struct object_t
{
    struct ll_node_t _node;
    struct ll_context_t _symbols;
    coord_t _pos;
    char *_name;
    size_t _props;
};

struct symbol_t
{
    struct ll_node_t _node;
    coord_t _pos;
    char _sym;
};
int engine_draw_symbol_at(aoc_2d_engine_t _eng, coord_t *_pos, char *_sym);
struct symbol_t *eng_sym_create(struct object_t *_obj, coord_t *_rpos, char _c);
int move_symbol(aoc_2d_engine_t _eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir);
void eng_free_sym(void *_data);

struct engine_t
{
    coord_t _boundaries;
    coord_t _cursor;
    coord_t _cellDim;
    coord_t _cellSpace;
    char _void_sym;
    struct range_t _canvas;
    struct ll_context_t _objects;
    size_t _statLine;
    size_t _PrivStatLine;
    size_t _prompter;
    bool _mustDraw;
};
int engine_resize(struct engine_t *_eng, coord_t _ns);
int engine_move_cursor(struct engine_t *_eng, coord_t _coord);
int is_within_boundaries(aoc_2d_engine_t _eng, coord_t *_pos);
int put_pos(aoc_2d_engine_t _eng, coord_t *_pos, coord_t *_npos);

int engine_fill_hv_line(struct engine_t *_eng, coord_t *_start, coord_t *_end, AOC_2D_DIR _dir, const char *_c);
int move_cursor_until(struct engine_t *_eng, AOC_2D_DIR _dir, size_t _steps, coord_t *limit);

int check_boundaries(struct engine_t *_eng, coord_t _coord);

int draw_object(struct engine_t *_eng, struct object_t *_obj, char *_fmt);
int erase_object(struct engine_t *_eng, struct object_t *_obj);
int engine_draw_box(struct engine_t *_eng);

int move_pos(aoc_2d_engine_t _eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir);
int engine_cursor_log(struct engine_t *_eng);
int engine_cursor_stats(struct engine_t *_eng);
void aoc_engine_prompt_stats(aoc_2d_engine_t _eng);
int engine_cursor_private_next_stats(struct engine_t *_eng);
int engine_fill_drawing_area(struct engine_t *_eng);