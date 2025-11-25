#ifndef AOC_LL
#define AOC_LL

#include <stdbool.h>

#define LL_MAX_LEN (65535)
#define NODE_CAST(_p) ((struct ll_node_t *)_p)

struct ll_context_t
{
    struct ll_node_t *_first;
    struct ll_node_t *_current;
    struct ll_node_t *_last;
    size_t _size;
};

typedef struct ll_context_t *aoc_linked_list_handle_t;

struct ll_node_t
{
    struct ll_node_t *_prev;
    struct ll_node_t *_next;
    void (*free)(void *_data);
};

typedef struct ll_node_t *(ll_compare)(struct ll_node_t * _a, struct ll_node_t *_b);

void ll_blk_init(aoc_linked_list_handle_t _blk);
void ll_blk_sort(aoc_linked_list_handle_t _blk, ll_compare(*comp));
int ll_node_append(aoc_linked_list_handle_t _blk, struct ll_node_t *_new);
void ll_free_all(aoc_linked_list_handle_t _blk, void (*_caller)(void *_data));
void ll_node_permut(aoc_linked_list_handle_t _ctx, struct ll_node_t *_a, struct ll_node_t *_b);
void ll_node_insert(aoc_linked_list_handle_t _ctx, struct ll_node_t *_a, struct ll_node_t *_b);
void ll_node_remove(aoc_linked_list_handle_t _ctx, struct ll_node_t *_a);
size_t aoc_ll_size(aoc_linked_list_handle_t _ctx);
int ll_find_node(aoc_linked_list_handle_t _blk, struct ll_node_t *_a);
struct ll_node_t *ll_find_node_by_property(aoc_linked_list_handle_t _blk, void *_prop, bool (*equal)(void *_a, void *_b));
size_t ll_count_nodes_by_property(aoc_linked_list_handle_t _blk, void *_prop, bool (*equal)(void *_a, void *_b));

#define LL_DECLARE(_type) \
    struct _type *_prev;  \
    struct _type *_next;

#define LL_CTOR(_type, _name)                              \
    static struct _type *_name(struct _type *_prev)        \
    {                                                      \
        struct _type *_ret = malloc(sizeof(struct _type)); \
        _ret->_prev = _prev;                               \
        if (_prev)                                         \
            _prev->_next = _ret;                           \
        _ret->_next = NULL;                                \
        return _ret;                                       \
    }

#define LL_FOREACH_P(_it, _pblk)                             \
    struct ll_node_t *_it;                                   \
    for (_it = _pblk->_first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH(_it, _blk)                              \
    struct ll_node_t *_it;                                 \
    for (_it = _blk._first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH_EXT(_it, _blk)                          \
    for (_it = _blk._first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH_LAST_EXT(_it, _blk)                    \
    for (_it = _blk._last; _it != NULL; _it = _it->_prev) \
        if (_it)

#define LL_FOREACH_LAST(_it, _blk) \
    struct ll_node_t *_it;         \
    LL_FOREACH_LAST_EXT(_it, _blk)

#endif