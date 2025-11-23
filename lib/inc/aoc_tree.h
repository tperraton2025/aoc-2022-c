#ifndef AOC_TREE
#define AOC_TREE

#include <aoc_linked_list.h>

#define TREE_MAX_CHILDREN (2048)
#define TREE_NODE_CAST(_p) ((struct tree_node_t *)_p)

struct tree_node_t
{
    struct tree_node_t *_parent;
    struct ll_context_t _children;
    void (*free)(void *_data);
    void *_data;
};

struct tree_blk_t
{
    struct tree_node_t *_root;
    size_t _size;
    void *_data;
};

struct tree_node_t *aoc_tree_node(struct tree_node_t *parent);
void tree_blk_init(struct tree_blk_t *_blk);
int tree_node_append(struct tree_node_t *_parent, struct tree_node_t *_new);
void tree_free_all(struct tree_blk_t *_blk, void (*_caller)(void *_data));
void aoc_tree_permut(struct tree_blk_t *_ctx, struct tree_node_t *_a, struct tree_node_t *_b);
void aoc_tree_insert(struct tree_blk_t *_ctx, struct tree_node_t *_a, struct tree_node_t *_b);
void aoc_tree_remove(struct tree_blk_t *_ctx, struct tree_node_t *_a);
size_t aoc_tree_size(struct tree_blk_t *_ctx);

#endif