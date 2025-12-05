
#include "aoc_ranges.h"
#include "aoc_types.h"
#include <stdbool.h>
#include <aoc_linked_list.h>


typedef enum
{
    OBJ_PROPERTY_STATIC = 0,
    OBJ_PROPERTY_MOBILE,
    OBJ_PROPERTY_START,
    OBJ_PROPERTY_END,
    OBJ_PROPERTY_NO_COLLISION,
    OBJ_PROPERTY_MAX
} object_properties_flags_t;


typedef struct ascii_2d_engine *aoc_2d_engine_h;
typedef struct object *aoc_2d_object_h;

typedef struct object
{
    struct dll_node _node;
    struct dll_head _parts;
    coord_t _pos;
    char *_name;
    size_t _props;
    size_t _refcnt;
} object_t;

typedef struct part
{
    struct dll_node _node;
    aoc_2d_object_h _parent;
    coord_t _pos;
    char _sym;
    char *_fmt;
} part_t;

typedef struct part *part_h; 

struct ascii_2d_engine
{
    coordboundaries_t _drawlimits;
    coordboundaries_t _coordlimits;
    coord_t _partoffset;
    size_t _newlinecnt;
    coord_t _cursor;
    char _voidsym;
    struct dll_head _objects;
    size_t _statLine;
    size_t _PrivStatLine;
    bool _enabledraw;
    bool _enablecollisions;
    size_t _delay;
};
 