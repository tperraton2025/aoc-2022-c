#include "aoc_helpers.h"
#include "aoc_tree.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TREE_NODE_CAST(_p) ((struct tree_node *)_p)

#define FOR_EACH_PARENT_UNTIL_ROOT(_pnode, _start) for (struct tree_node *_pnode = _start->_parent; _pnode; _pnode = _pnode->_parent)
#define TREE_FIND_ROOT_EXT(_pnode, _start) for (_pnode = _start; _pnode->_parent; _pnode = _pnode->_parent)

int aoc_tree_node(tree_node_h parent_node, tree_node_h new_node, void (*free)(void *arg))
{
    assert(free);
    assert(new_node);
    memset(new_node, 0, sizeof(struct tree_node));

    new_node->_free = free;
    new_node->_parent = parent_node;

    dll_head_init(&new_node->_dllchildren);

    if (parent_node)
        aoc_tree_node_append(parent_node, new_node);
    return 0;
}

int aoc_tree_node_append(tree_node_h _parent, tree_node_h _new)
{
    if (_parent == _new)
        return EINVAL;
    int ret = dll_node_append(&TREE_NODE_CAST(_parent)->_dllchildren, &_new->_llnode);
    if (ret)
        return ret;

    _new->_parent = _parent;
    FOR_EACH_PARENT_UNTIL_ROOT(_node, _new)
    {
        _node->_size++;
    }
    return 0;
}

void aoc_tree_free_all_children(tree_node_h _start)
{
    assert(_start && "NULL pointer detected");

    struct dll_head *_ll = &_start->_dllchildren;
    dll_node_h _node;
    dll_node_h _nxtNode;
    for (_node = _ll->_first; _node;)
    {
        _nxtNode = _node->_next;
        aoc_tree_free_all_children(TREE_NODE_CAST(_node));
        _node = _nxtNode;
    }
    _start->_free(_start);
    FREE(_start);
}

void aoc_tree_free(tree_node_h _start)
{
    assert(_start && "No starting node method provided");

    tree_node_h _root = aoc_tree_find_root(_start);

    struct dll_head *_ll = &_root->_dllchildren;
    dll_node_h _node;
    dll_node_h _nxtNode;

    for (_node = _ll->_first; _node;)
    {
        _nxtNode = _node->_next;
        aoc_tree_free_all_children(TREE_NODE_CAST(_node));
        _node = _nxtNode;
    }
    _root->_free(_root);
}

int aoc_tree_swap(tree_node_h _a, tree_node_h _b)
{
    if (!_a || !_b)
        return EINVAL;

    struct tree_node *_anode = TREE_NODE_CAST(_a);
    struct tree_node *_bnode = TREE_NODE_CAST(_b);

    dll_node_disconnect(&_anode->_parent->_dllchildren, NODE_CAST(_a));
    dll_node_disconnect(&_bnode->_parent->_dllchildren, NODE_CAST(_b));

    dll_node_append(&_bnode->_parent->_dllchildren, NODE_CAST(_a));
    dll_node_append(&_anode->_parent->_dllchildren, NODE_CAST(_b));

    _anode->_parent = _bnode->_parent;
    _bnode->_parent = _anode;
    return 0;
}

static bool pointer_comp(void *const _a, void *const _b)
{
    return _a == _b;
}

void aoc_tree_free_node(tree_node_h _a)
{
    assert(_a && "NULL pointer in args");
    tree_node_h _root = NULL;
    TREE_FIND_ROOT_EXT(_root, _a);
    tree_node_h _to_delete = aoc_tree_search_node_by_property(_root, _a, pointer_comp);
    assert(_to_delete && "attempt to delete a non-existing tree node");

    struct tree_node *_anode = TREE_NODE_CAST(_a);

    /* Takes out node from the list without freeing it */
    if (_anode->_parent)
        dll_node_disconnect(&_anode->_parent->_dllchildren, NODE_CAST(_a));

    aoc_tree_free_all_children(_a);
    FOR_EACH_PARENT_UNTIL_ROOT(_node, _to_delete)
    {
        assert(_node->_size && "error in tree node counting");
        _node->_size--;
    }
}

tree_node_h aoc_tree_leaf_node(tree_node_h _start)
{
    struct tree_node *ret = NULL;
    assert(_start && "tree node is NULL");
    while (_start->_dllchildren._first)
    {
        ret = (tree_node_h)_start->_dllchildren._first;
    }
    return ret;
}

/* TODO: reimplement without recursion */
tree_node_h aoc_tree_search_node_by_property(tree_node_h _start, void *_prop, bool (*_equal)(void *_a, void *_b))
{
    tree_node_h _item = NULL;
    assert(_start && _prop && _equal && "Invalid NULL parameters");
    if (_equal(_start, _prop))
    {
        return _start;
    }
    LL_FOREACH(_node, _start->_dllchildren)
    {
        _item = aoc_tree_search_node_by_property(TREE_NODE_CAST(_node), _prop, _equal);
        if (_item)
            return TREE_NODE_CAST(_item);
    }
    return NULL;
}

size_t aoc_tree_size(tree_node_h _start)
{
    tree_node_h _root = NULL;
    TREE_FIND_ROOT_EXT(_root, _start);
    assert(_root && "_root found is NULL");
    return _root->_size;
}

tree_node_h aoc_tree_find_root(tree_node_h _start)
{
    tree_node_h _root = _start;
    TREE_FIND_ROOT_EXT(_root, _start);
    return _root;
}

void aoc_tree_foreach_node(tree_node_h _start, void (*func)(tree_node_h _a))
{
    func(_start);
    LL_FOREACH(_subnodes, _start->_dllchildren)
    {
        aoc_tree_foreach_node((tree_node_h)_subnodes, func);
    }
}

void aoc_tree_foreach_nodes_arg(tree_node_h start, void *arg, void (*func)(void *arg, tree_node_h _a))
{
    func(arg, start);
    LL_FOREACH(_subnodes, start->_dllchildren)
    {
        aoc_tree_foreach_nodes_arg((tree_node_h)_subnodes, arg, func);
    }
}

void aoc_tree_foreach_parent_until_root(tree_node_h _start, void (*func)(tree_node_h _a))
{
    func(_start);
    FOR_EACH_PARENT_UNTIL_ROOT(_tnode, _start->_parent)
    {
        func((tree_node_h)_tnode);
    }
}