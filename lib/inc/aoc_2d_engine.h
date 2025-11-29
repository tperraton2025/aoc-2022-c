#include <errno.h>
#include "aoc_types.h"
#include "ansi_cmd.h"
#include "aoc_ranges.h"
#include "aoc_linked_list.h"

#define OBJ_NAME_FMT "%16s"

#define ABSOLUTE_MAX_X (4096)
#define ABSOLUTE_MAX_Y (4096)

#define ABSOLUTE_MAX_SYM_CNT (32)
#define ABSOLUTE_MAX_NAME_LEN (16)
#define ABSOLUTE_MAX_STAT_LEN (128)
#define ABSOLUTE_MAX_STATS_LINES (128)

#define BOUNDARY_CHECK(_a, _b) (_a._x <= _b._x) && (_a._y <= _b._y)
#define BOUNDARY_CHECK_P(_a, _b) (_a->_x <= _b->_x) && (_a->_y <= _b->_y)

#define SYM_CAST(_p) ((struct symbol_t *)_p)
#define aoc_engine_prompt_multistr(_eng, _sleep, ...)                                 \
    {                                                                                 \
        const char *_str[] = {__VA_ARGS__};                                           \
        aoc_engine_prompt(_eng, _sleep, sizeof(_str) / sizeof(_str[0]), __VA_ARGS__); \
    }

#define aoc_engine_prompt_extra_stats_as_err(_eng, _fmt, ...)        \
    {                                                                \
        engine_cursor_user_next_stats(_eng);                         \
        printf(ERASE_LINE_FROM_CR RED _fmt RESET "\n", __VA_ARGS__); \
        scanf("%*c");                                                \
    }

typedef enum
{
    OBJ_PROPERTY_STATIC = 0,
    OBJ_PROPERTY_MOBILE,
    OBJ_PROPERTY_START,
    OBJ_PROPERTY_END,
    OBJ_PROPERTY_NO_COLLISION,
    OBJ_PROPERTY_MAX
} object_properties_flags_t;

typedef struct coord
{
    size_t _x;
    size_t _y;
} coord_t;

typedef struct ascii_2d_engine *aoc_2d_engine_h;
typedef struct object *aoc_2d_object_h;

aoc_2d_engine_h engine_create(coord_t _res, coord_t _spce, char _void_sym);
void eng_obj_free(void *_data);
void eng_set_refresh_delay(aoc_2d_engine_h _eng, size_t delay);
int engine_draw_objects(aoc_2d_engine_h _eng, coord_t *_corner);
int draw_object(struct ascii_2d_engine *_eng, struct object *_obj, char *_fmt);

int aoc_engine_resize_one_direction(aoc_2d_engine_h _eng, size_t steps, AOC_2D_DIR _dir);
void engine_free(aoc_2d_engine_h _eng);
int engine_draw(aoc_2d_engine_h _eng);

aoc_2d_object_h eng_obj_create(aoc_2d_engine_h _eng, const char *const _name, coord_t *_pos, char *_sym, size_t _props);
int aoc_engine_append_obj(aoc_2d_engine_h _eng, aoc_2d_object_h _obj);
aoc_2d_object_h aoc_engine_get_obj_my_name(aoc_2d_engine_h _eng, const char *name);
aoc_2d_object_h aoc_engine_get_obj_by_position(aoc_2d_engine_h _eng, coord_t *_pos);
const char *const aoc_engine_get_obj_name(aoc_2d_object_h _obj);

int aoc_engine_move_object(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t _steps, AOC_2D_DIR dir);
int aoc_engine_step_object_and_redraw(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, AOC_2D_DIR dir, char *_fmt);
int aoc_engine_move_object_and_redraw(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t steps, AOC_2D_DIR dir);
int aoc_engine_put_object_and_redraw(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, coord_t _npos);

coord_t aoc_engine_get_boundaries(aoc_2d_engine_h _eng);
coord_t aoc_engine_get_object_position(aoc_2d_engine_h _eng, aoc_2d_object_h _obj);
size_t aoc_engine_get_XY_moves_between_objects(aoc_2d_engine_h _eng, aoc_2d_object_h _a, aoc_2d_object_h _b);
size_t aoc_engine_get_XYD_moves_between_objects(aoc_2d_engine_h _eng, aoc_2d_object_h _a, aoc_2d_object_h _b);

void aoc_engine_list_objects(aoc_2d_engine_h _eng);
int engine_put_cursor_in_footer_area(struct ascii_2d_engine *_eng);
int engine_cursor_user_next_stats(struct ascii_2d_engine *_eng);
int engine_cursor_user_stats(struct ascii_2d_engine *_eng);

int engine_cursor_exit_drawing_area(struct ascii_2d_engine *_eng);

int engine_draw_symbol_at(aoc_2d_engine_h _eng, coord_t *_pos, char *_sym);
size_t aoc_engine_get_dist_between_objects(aoc_2d_engine_h _eng, aoc_2d_object_h _a, aoc_2d_object_h _b);
int aoc_engine_move_one_step_towards(aoc_2d_engine_h _eng, aoc_2d_object_h _a, coord_t _pos);

void engine_clear_screen();
void engine_activate_drawing(aoc_2d_engine_h _eng);
void engine_deactivate_drawing(aoc_2d_engine_h _eng);
int move_pos(aoc_2d_engine_h _eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir);

int aoc_inputs_ansi_to_dir(const char *const _str, AOC_2D_DIR *_dir);
void aoc_engine_prompt(aoc_2d_engine_h _eng, const size_t _sleep, size_t _count, ...);