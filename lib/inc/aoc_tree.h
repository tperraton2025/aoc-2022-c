#ifndef AOC_TREE
#define AOC_TREE

#include <aoc_linked_list.h>

#define TREE_MAX_CHILDREN (2048)

struct tree_node_t
{
    struct ll_node_t _node;
    struct tree_node_t *_parent;
    struct ll_context_t _children;
    void (*_free)(void *_data);
};

typedef struct tree_node_t *aoc_tree_node_h;
typedef struct tree_blk_t *aoc_tree_blk_t;

int aoc_tree_node(aoc_tree_node_h parent, aoc_tree_node_h new, void (free)(void *arg));
aoc_tree_node_h tree_blk_init(void);
int tree_node_append(aoc_tree_node_h _parent, aoc_tree_node_h _new);

void tree_free_all(aoc_tree_blk_t _blk, aoc_tree_node_h _start, void (_caller)(void *_data));
void tree_free_all_from_starting_node(aoc_tree_node_h _start, void (_caller)(void *_data));

void aoc_tree_free_node(aoc_tree_node_h _a, void (_free)(void *arg));
void aoc_tree_permut(aoc_tree_node_h _a, aoc_tree_node_h _b);
size_t aoc_tree_size(aoc_tree_node_h _start);

aoc_tree_node_h aoc_tree_leaf_node(aoc_tree_node_h _parent);
aoc_tree_node_h aoc_tree_search_node(aoc_tree_blk_t _ctx, aoc_tree_node_h _a);
aoc_tree_node_h aoc_tree_search_node_by_property(aoc_tree_node_h _start, void *_prop, bool (*_equal)(void * _a, void * _b));
#endif