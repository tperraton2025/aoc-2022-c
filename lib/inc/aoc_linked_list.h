#ifndef AOC_LL
#define AOC_LL

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define LL_MAX_LEN (65535)
#define NODE_CAST(_p) ((struct dll_node *)_p)
#define MAX_STR_DLL_LEN (1024)

struct dll_head
{
    struct dll_node *_first;
    struct dll_node *_current;
    struct dll_node *_last;
    size_t _size;
};

typedef struct dll_head *dll_head_h;
typedef struct dll_head dll_head_t;

struct dll_node
{
    struct dll_node *_prev;
    struct dll_node *_next;
    void (*free)(void *_data);
};

typedef struct string_dll_node
{
    struct dll_node _node;
    char *_str;
} string_dll_node_t;

typedef struct dll_node *(dll_compare)(struct dll_node * _a, struct dll_node *_b);

typedef struct string_dll_node *string_dll_node_h;

void dll_head_init(dll_head_h head);
void dll_sort(dll_head_h head, dll_compare(*comp));
void dll_free_all(dll_head_h head, void (*_caller)(void *_data));

int dll_node_append(dll_head_h head, struct dll_node *_new);
int dll_find_node(dll_head_h head, struct dll_node *_a);
int dll_node_sorted_insert(dll_head_h head, struct dll_node *_new, dll_compare sort);

void dll_node_permut(dll_head_h head, struct dll_node *_a, struct dll_node *_b);
void dll_node_insert(dll_head_h head, struct dll_node *_a, struct dll_node *_b);
void dll_node_disconnect(dll_head_h head, struct dll_node *_a);

size_t dll_size(dll_head_h head);
size_t dll_count_nodes_by_property(dll_head_h head, void *_prop, bool (*equal)(void *_a, void *_b));

struct dll_node *dll_find_node_by_property(dll_head_h head, void *_prop, bool (*equal)(void *_a, void *_b));

#define DLL_DECLARE(_type) \
    struct dll_node _node;

#define DLL_NODE_CTOR(_type, _name)           \
    static _type *_name(void)                 \
    {                                         \
        _type *_ret = NULL;                   \
        TRY_RAII_MALLOC(_ret, sizeof(_type)); \
        if (!_ret)                            \
            return NULL;                      \
        _ret->_node._prev = NULL;             \
        _ret->_node._next = NULL;             \
        return _ret;                          \
    }

#define LL_FOREACH_P_EXT(_it, _head_h)                         \
    for (_it = _head_h->_first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH_P(_it, _head_h)                             \
    struct dll_node *_it;                                      \
    for (_it = _head_h->_first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH(_it, _head)                              \
    struct dll_node *_it;                                   \
    for (_it = _head._first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH_EXT(_it, _head)                          \
    for (_it = _head._first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH_LAST_EXT(_it, _head)                    \
    for (_it = _head._last; _it != NULL; _it = _it->_prev) \
        if (_it)

#define LL_FOREACH_LAST(_it, _blk) \
    struct dll_node *_it;          \
    LL_FOREACH_LAST_EXT(_it, _blk)

void string_dll_free(void *string);

string_dll_node_h string_dll(const char *const name);

struct dll_node *string_dll_compare(struct dll_node *_a, struct dll_node *_b);

int string_dll_rename(string_dll_node_h node, const char *const name);

#endif