#ifndef AOC_TREE
#define AOC_TREE

#include <aoc_linked_list.h>

#define TREE_MAX_CHILDREN (2048)

struct tree_node
{
    struct dll_node _llnode;
    struct tree_node *_parent;
    struct dll_head _dllchildren;
    void (*_free)(void *_data);
    size_t _size;
};

typedef struct tree_node *aoc_tree_node_h;

typedef bool (*tree_node_compare)(void *_a, void *_b);

int aoc_tree_node(aoc_tree_node_h parent, aoc_tree_node_h new, void(free)(void *arg));
int aoc_tree_node_append(aoc_tree_node_h _parent, aoc_tree_node_h _new);

void aoc_tree_free_all_children(aoc_tree_node_h _start);

void aoc_tree_free_node(aoc_tree_node_h _a);
aoc_tree_node_h aoc_tree_find_root(aoc_tree_node_h _start);
void aoc_tree_free(aoc_tree_node_h _start);

int aoc_tree_permut(aoc_tree_node_h _a, aoc_tree_node_h _b);
size_t aoc_tree_size(aoc_tree_node_h _start);

aoc_tree_node_h aoc_tree_leaf_node(aoc_tree_node_h _parent);
aoc_tree_node_h aoc_tree_search_node_by_property(aoc_tree_node_h _start, void *_prop, bool (*_equal)(void *_a, void *_b));

void aoc_tree_foreach_node(aoc_tree_node_h _start, void (*func)(aoc_tree_node_h _a));
void aoc_tree_foreach_parent_until_root(aoc_tree_node_h _start, void (*func)(aoc_tree_node_h _a));
void aoc_tree_foreach_nodes_arg(aoc_tree_node_h _start, void *_output, void (*func)(void *_output, aoc_tree_node_h _a));

#endif