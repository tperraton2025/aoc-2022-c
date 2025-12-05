#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <aoc_numeric.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <aoc_tree.h>
#include <unistd.h>
#include <aoc_helpers.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct joltage
{
    DLL_DECLARE()
    size_t _rating;
    size_t _order;
} joltage_t;
typedef struct joltage *joltage_h;

joltage_h joltage_ctor();

static bool hasexactjoltagerating(void *arg, void *prop)
{
    joltage_h _jolta = (joltage_h)arg;
    return _jolta->_rating == *(size_t *)prop;
}

static bool isajoltagethatcameafter(void *arg, void *prop)
{
    joltage_h _jolta = (joltage_h)arg;
    return _jolta->_order > *(size_t *)prop;
}

static dll_node_h joltagesortbyorder(dll_node_h arga, dll_node_h argb)
{
    joltage_h _jolta = (joltage_h)arga;
    joltage_h _joltb = (joltage_h)argb;
    return _jolta->_order > _joltb->_order ? &_joltb->_node : &_jolta->_node;
}

static bool isnotajoltagethatcameafter(void *arg, void *prop)
{
    joltage_h _jolta = (joltage_h)arg;
    return _jolta->_order <= *(size_t *)prop;
}

static dll_node_h joltagecomparebyrating(dll_node_h arga, dll_node_h argb)
{
    joltage_h _jolta = (joltage_h)arga;
    joltage_h _joltb = (joltage_h)argb;
    return _jolta->_rating > _joltb->_rating ? &_jolta->_node : &_joltb->_node;
}

static void populatejoltageratings(dll_head_h head, char *_str)
{
    joltage_t _joltagetmp = {0};
    for (size_t _ii = 0; _ii < strlen(_str); _ii++)
    {
        if (1 == sscanf(_str + _ii, "%1lu", &_joltagetmp._rating))
        {
            joltage_h _njolt = joltage_ctor();
            _njolt->_rating = _joltagetmp._rating;
            _njolt->_order = _ii;
            dll_node_sorted_insert(head, &_njolt->_node, joltagecomparebyrating);
        }
    }
}

typedef struct
{
    tree_node_t _treenode;
    dll_head_h _joltages;
    base10_h _conversion;
    size_t _depth;
} joltchoice_t;

typedef joltchoice_t *joltchoice_h;

static void freejoltchoice(void *arg)
{
    joltchoice_h _choice = (joltchoice_h)arg;
    free(_choice->_conversion);
    dll_free_all(_choice->_joltages, free);
    free(_choice->_joltages);
}

static joltchoice_h newjoltchoice(joltchoice_h parent)
{
    joltchoice_h _choice = malloc(sizeof(joltchoice_t));
    if (!parent)
    {
        _choice->_joltages = malloc(sizeof(dll_head_t));
        dll_head_init(_choice->_joltages);
    }
    _choice->_conversion = base_10_conversion(0);
    _choice->_conversion->_lastdigit = 12;

    if (parent)
        memcpy(_choice->_conversion, parent->_conversion, sizeof(base10_t));
    aoc_tree_node(parent ? &parent->_treenode : NULL, &_choice->_treenode, freejoltchoice);
    _choice->_depth = parent ? parent->_depth - 1 : 12;
}

static void printyolfmt(void *arga, void *argb)
{
    joltage_h _a = (joltage_h)arga;
    char *_b = (char *)argb;
    printf("%s%lu" RESET, _b, _a->_rating);
}

static joltchoice_h makenextjoltchoice(joltchoice_h parent, joltage_h from)
{
    joltchoice_h _subchoice = newjoltchoice(parent);
    _subchoice->_joltages = dll_clone_trimmed(parent->_joltages, sizeof(joltage_t), (void *)&from->_order, isnotajoltagethatcameafter);
    dll_foreach(_subchoice->_joltages, GREEN, printyolfmt);
    printf("\n");
    if (!_subchoice->_joltages)
        goto abort;
    if (_subchoice->_joltages->_size < _subchoice->_depth)
        goto abort;

    _subchoice->_conversion->_digits[_subchoice->_depth] = from->_rating;

    if (!_subchoice->_depth)
    {
        base_10_reverse_conversion(_subchoice->_conversion);
        joltchoice_h _parent = parent;
        while (_parent->_treenode._parent)
            _parent = (joltchoice_h)_parent->_treenode._parent;
        if (_parent->_conversion->_val < _subchoice->_conversion->_val)
        {
            aoc_info("got a conversion %lu", _subchoice->_conversion->_val);
            _parent->_conversion->_val = _subchoice->_conversion->_val;
        }
        // else
        // aoc_info("got a conversion %lu < %lu", _subchoice->_conversion->_val, _parent->_conversion->_val);
    }
    else
    {
        LL_FOREACH_P(_node, _subchoice->_joltages)
        {
            joltage_h _jolth = (joltage_h)_node;
            joltchoice_h _njolth = makenextjoltchoice(_subchoice, _jolth);
            if (!_njolth)
                continue;
        }
    }
    return _subchoice;
abort:
    return NULL;
}