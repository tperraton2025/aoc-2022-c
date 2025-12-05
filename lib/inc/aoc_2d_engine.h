#include <errno.h>
#include "aoc_types.h"
#include "ansi_cmd.h"
#include "aoc_ranges.h"
#include "aoc_linked_list.h"

#define MAX_NAME_LEN_LUI (16LU)
#define MAX_NAME_LEN_STR "16"
#define OBJ_NAME_FMT "%" MAX_NAME_LEN_STR "s"

#define ABSOLUTE_MAX_X (4096)
#define ABSOLUTE_MAX_Y (4096)

#define DRAWABLE_MAX_X (64)
#define DRAWABLE_MAX_Y (128)

#define ABSOLUTE_MAX_PART_CNT (32)
#define ABSOLUTE_MAX_PART_FMT_LEN (16)

#define ABSOLUTE_MAX_STAT_LEN (128) f
#define ABSOLUTE_MAX_STATS_LINES (128)

#define COORD_RANGE_CHECK(_coord, _range) (N_BETWEEN_AB(_coord._x, _range._min._x, _range._max._x) && N_BETWEEN_AB(_coord._y, _range._min._y, _range._max._y))
#define COORD_RANGE_CHECK_P(_coord, _range) (N_BETWEEN_AB(_coord->_x, _range._min._x, _range._max._x) && N_BETWEEN_AB(_coord->_y, _range._min._y, _range._max._y))

#define COORD_MAXIMA_CHECK(_a, _b) (_a._x <= _b._x) && (_a._y <= _b._y)
#define BOUNDARY_CHECK_P(_a, _b) (_a->_x <= _b->_x) && (_a->_y <= _b->_y)

#define SYM_CAST(_p) ((part_h)_p)
#define aoc_engine_prompt_multistr(_eng, _sleep, ...)                                 \
    {                                                                                 \
        const char *_str[] = {__VA_ARGS__};                                           \
        aoc_engine_prompt(_eng, _sleep, sizeof(_str) / sizeof(_str[0]), __VA_ARGS__); \
    }

#define aoc_engine_prompt_extra_stats_as_err(_eng, _fmt, ...)        \
    {                                                                \
        engine_cursor_user_next_stats(_eng);                         \
        printf(ERASE_LINE_FROM_CR RED _fmt RESET "\n", __VA_ARGS__); \
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

typedef struct coordboundaries
{
    coord_t _min;
    coord_t _max;
} coordboundaries_t;

typedef struct move
{
    int _x;
    int _y;
} move_t;

typedef struct ascii_2d_engine *aoc_2d_engine_h;
typedef struct object *aoc_2d_object_h;

aoc_2d_engine_h engine_create(coord_t *partcoordmaxima, char _voidsym, size_t delay);
void aoc_engine_free_object(void *_data);
void eng_set_refresh_delay(aoc_2d_engine_h _eng, size_t delay);
int engine_draw_objects(aoc_2d_engine_h _eng, coord_t *_corner);
int aoc_engine_draw_object(struct ascii_2d_engine *eng, struct object *obj, char *specfmt);

int aoc_engine_extend_one_direction(aoc_2d_engine_h _eng, size_t steps, AOC_2D_DIR _dir);
void engine_free(aoc_2d_engine_h _eng);
int engine_draw(aoc_2d_engine_h _eng);

aoc_2d_object_h aoc_engine_object(aoc_2d_engine_h eng, const char *const name, coord_t *pos, char *sym, size_t props);
int aoc_engine_append_obj(aoc_2d_engine_h _eng, aoc_2d_object_h _obj);
aoc_2d_object_h aoc_engine_get_obj_my_name(aoc_2d_engine_h _eng, const char *name);
aoc_2d_object_h aoc_engine_get_obj_by_position(aoc_2d_engine_h _eng, coord_t *_pos);
size_t aoc_engine_get_object_count(aoc_2d_engine_h _eng);
const char *const aoc_engine_get_obj_name(aoc_2d_object_h _obj);

int aoc_engine_move_object(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t _steps, AOC_2D_DIR dir);
int aoc_engine_step_object(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t steps, AOC_2D_DIR dir, char *_fmt);
int aoc_engine_move_object_and_redraw(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t steps, AOC_2D_DIR dir);
int aoc_engine_put_object_and_redraw(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, coord_t _npos);
int aoc_engine_collision_at(aoc_2d_engine_h _eng, aoc_2d_object_h excl, coord_t *_pos);

coordboundaries_t aoc_engine_get_parts_boundaries(aoc_2d_engine_h _eng);
coord_t aoc_engine_get_object_position(aoc_2d_engine_h _eng, aoc_2d_object_h _obj);
size_t aoc_engine_get_XY_moves_between_objects(aoc_2d_engine_h _eng, aoc_2d_object_h _a, aoc_2d_object_h _b);
size_t aoc_engine_get_XYD_moves_between_objects(aoc_2d_engine_h _eng, aoc_2d_object_h _a, aoc_2d_object_h _b);

void aoc_engine_prompt_obj_list(aoc_2d_engine_h _eng);
int engine_put_cursor_in_footer_area(struct ascii_2d_engine *_eng);
int engine_cursor_user_next_stats(struct ascii_2d_engine *_eng);
int engine_cursor_user_stats(struct ascii_2d_engine *_eng);

int engine_cursor_exit_drawing_area(struct ascii_2d_engine *_eng);

int engine_draw_part_at(aoc_2d_engine_h _eng, coord_t *_pos, char *_sym);
int engine_draw_symbol_at(aoc_2d_engine_h eng, coord_t *_pos, const char *_sym);
size_t aoc_engine_get_dist_between_objects(aoc_2d_engine_h _eng, aoc_2d_object_h _a, aoc_2d_object_h _b);
int aoc_engine_move_one_step_towards(aoc_2d_engine_h _eng, aoc_2d_object_h _a, coord_t _pos);

void engine_clear_screen();
void engine_activate_drawing(aoc_2d_engine_h _eng);
void engine_deactivate_drawing(aoc_2d_engine_h _eng);
int move_within_window(aoc_2d_engine_h _eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir);

int aoc_inputs_ansi_to_dir(const char *const _str, AOC_2D_DIR *_dir);
void aoc_engine_prompt(aoc_2d_engine_h _eng, const size_t _sleep, size_t _count, ...);

int aoc_engine_foreach_object_arg(aoc_2d_engine_h _eng, void *arg, void func(coord_t *pos, void *arg));

typedef struct aoc_2d_object_ref
{
    struct dll_node _node;
    aoc_2d_object_h data;
    int _blocked;
} aoc_2d_object_ref_t;

typedef struct aoc_2d_object_ref *aoc_2d_object_ref_h;

aoc_2d_object_ref_h aoc_2d_object_ref(aoc_2d_object_h _obj);
void aoc_2d_object_ref_free(void *arg);

struct dll_head *aoc_engine_prompt_obj_list_with_a_part_at_position(aoc_2d_engine_h _eng, coord_t *_pos);

char *strpos(coord_t *pos);
char *strobjcnt(aoc_2d_engine_h _eng);

void engine_add_line(aoc_2d_engine_h _eng);
size_t engine_last_line(aoc_2d_engine_h _eng);

dll_head_h engine_get_objects_positions(aoc_2d_engine_h _eng);

typedef struct coord_tracker
{
    struct dll_node _node;
    coord_t _coord;
} coord_tracker_t;

typedef coord_tracker_t *coord_tracker_h;
coord_tracker_h coordtracker_ctor();

bool coord_compare(void *_a, void *_b);
int engine_remove_object(aoc_2d_engine_h eng, aoc_2d_object_h obj);