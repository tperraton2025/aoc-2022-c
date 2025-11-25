#include "aoc_helpers.h"
#include "aoc_tree.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TREE_NODE_CAST(_p) ((struct tree_node_t *)_p)
#define TREE_BLCK_CAST(_p) ((struct tree_blk_t *)_p)

struct tree_blk_t
{
    struct tree_node_t _root;
    size_t _size;
};

static struct tree_blk_t *aoc_tree_find_root(aoc_tree_node_h _start);

aoc_tree_node_h tree_blk_init(void)
{
    struct tree_blk_t *_blk = malloc(sizeof(struct tree_blk_t));
    memset(_blk, 0, sizeof(struct tree_blk_t));

    aoc_tree_node(NULL, &_blk->_root, free);
    _blk->_size = 0;
    return &_blk->_root;
}

int aoc_tree_node(aoc_tree_node_h parent, aoc_tree_node_h new, void (*free)(void *arg))
{
    memset(new, 0, sizeof(struct tree_node_t));

    new->_free = free;
    new->_parent = parent;
    ll_blk_init(&new->_children);

    if (parent)
        tree_node_append(parent, new);
    return 0;
}

int tree_node_append(aoc_tree_node_h _parent, aoc_tree_node_h _new)
{
    if (_parent == _new)
        return EINVAL;
    ll_node_append(&TREE_NODE_CAST(_parent)->_children, NODE_CAST(_new));
    _new->_parent = _parent;
    aoc_tree_find_root(_parent)->_size++;
}

void tree_free_all(aoc_tree_blk_t _blk, aoc_tree_node_h _start, void(_caller)(void *_data))
{
    assert(_blk && "No tree control block found");
    assert(_caller && "No free method provided");
    assert(_start && "No free method provided");

    LL_FOREACH(_node, _start->_children)
    {
        tree_free_all(_blk, TREE_NODE_CAST(_node), _caller);
    }
    ll_free_all(&_start->_children, _caller);
    _caller(_start);
    free(_blk);
}

void tree_free_all_children(aoc_tree_node_h _start, void(_caller)(void *_data))
{
    assert(_start && _caller && "NULL pointer detected");

    LL_FOREACH(_node, _start->_children)
    {
        tree_free_all_children(TREE_NODE_CAST(_node), _caller);
    }
    ll_free_all(&_start->_children, _caller);
}

void tree_free_all_from_starting_node(aoc_tree_node_h _start, void(_caller)(void *_data))
{
    assert(_start && _caller && "NULL pointer detected");
    struct tree_blk_t *_blk = aoc_tree_find_root(_start);
    assert(_blk && "No tree control block found from starting node");

    LL_FOREACH(_node, _blk->_root._children)
    {
        tree_free_all(_blk, TREE_NODE_CAST(_node), _caller);
    }
}

void aoc_tree_permut(aoc_tree_node_h _a, aoc_tree_node_h _b)
{
    struct tree_node_t *_anode = TREE_NODE_CAST(_a);
    struct tree_node_t *_bnode = TREE_NODE_CAST(_b);

    ll_node_remove(&_anode->_parent->_children, NODE_CAST(_a));
    ll_node_remove(&_bnode->_parent->_children, NODE_CAST(_b));

    ll_node_append(&_bnode->_parent->_children, NODE_CAST(_a));
    ll_node_append(&_anode->_parent->_children, NODE_CAST(_b));

    _anode->_parent = _bnode->_parent;
    _bnode->_parent = _anode;
}

static bool pointer_comp(void *const _a, void *const _b)
{
    return _a == _b;
}

void aoc_tree_free_node(aoc_tree_node_h _a, void(_free)(void *arg))
{
    assert(_a && _free && "NULL pointer in args");
    aoc_tree_blk_t _blk = aoc_tree_find_root(_a);
    assert(_blk && "provided node does not have a control block");
    aoc_tree_node_h _to_delete = aoc_tree_search_node_by_property(&_blk->_root, _a, pointer_comp);
    assert(_to_delete && "attempt to delete a non-existing tree node");

    struct tree_node_t *_anode = TREE_NODE_CAST(_a);

    if (_anode->_parent)
        ll_node_remove(&_anode->_parent->_children, NODE_CAST(_a));

    tree_free_all_children(_a, _free);
    assert(_blk->_size && "error in tree node counting");
    _free(_a);
    _blk->_size--;
}

aoc_tree_node_h aoc_tree_leaf_node(aoc_tree_node_h _start)
{
    struct tree_node_t *ret = NULL;
    assert(_start && "tree node is NULL");
    while (_start->_children._first)
    {
        ret = (aoc_tree_node_h)_start->_children._first;
    }
    return ret;
}

aoc_tree_node_h aoc_tree_search_node(aoc_tree_blk_t _ctx, aoc_tree_node_h _a)
{
}

/* TODO: reimplement without recursion */
aoc_tree_node_h aoc_tree_search_node_by_property(aoc_tree_node_h _start, void *_prop, bool (*_equal)(void *_a, void *_b))
{
    aoc_tree_node_h _item = NULL;
    assert(_start && _prop && _equal && "Invalid NULL parameters");
    if (_equal(_start, _prop))
    {
        return _start;
    }
    LL_FOREACH(_node, _start->_children)
    {
        _item = aoc_tree_search_node_by_property(TREE_NODE_CAST(_node), _prop, _equal);
        if (_item)
            return TREE_NODE_CAST(_item);
    }
    return NULL;
}

size_t aoc_tree_size(aoc_tree_node_h _start)
{
    return aoc_tree_find_root(_start)->_size;
}

struct tree_blk_t *aoc_tree_find_root(aoc_tree_node_h _start)
{
    struct tree_node_t *_srch = (struct tree_node_t *)_start;
    while (_srch->_parent)
        _srch = _srch->_parent;
    return TREE_BLCK_CAST(_srch);
}
