#include "aoc_helpers.h"
#include "aoc_linked_list.h"
#include <errno.h>
#include <assert.h>

void dll_head_init(aoc_ll_head_h head)
{
    head->_first = NULL;
    head->_current = NULL;
    head->_last = NULL;
    head->_size = 0;
}

void dll_free_all(aoc_ll_head_h head, void (*_caller)(void *_data))
{
    assert(head && "Linked list context is NULL");
    struct dll_node *_current = head->_first;
    if (!_current)
        goto end;
    struct dll_node *_next = _current->_next;
    while (_current)
    {
        _next = _current->_next;
        if (_caller)
            _caller(_current);
        _current = _next;
        head->_size--;
    }
end:
    head->_current = NULL;
    head->_first = NULL;
    head->_last = NULL;
    assert(head->_size == 0 && "the link list was probably broken");
}

size_t aoc_dll_size(aoc_ll_head_h head)
{
    assert(head);
    return head->_size;
}

int dll_node_append(aoc_ll_head_h head, struct dll_node *_new)
{
    assert(head && _new && "NULL pointer in dll_node_append");
    assert(head->_size < LL_MAX_LEN);

    head->_size++;
    if (head->_last)
    {
        _new->_prev = head->_last;
        head->_last->_next = _new;
        head->_last = _new;
    }
    else
    {
        head->_first = _new;
        head->_last = _new;
        _new->_prev = NULL;
    }
    head->_current = _new;
    _new->_next = NULL;
    return 0;
}

int dll_node_sorted_insert(aoc_ll_head_h head, struct dll_node *_new, dll_compare sort)
{
    assert(head && _new && "NULL pointer in dll_node_append");
    if (head->_size > LL_MAX_LEN)
        return ENOSPC;

    struct dll_node *_afterNew = NULL;
    LL_FOREACH_P_EXT(_afterNew, head)
    {
        if (_new == sort(_afterNew, _new))
        {
            break;
        }
    }
    if (_afterNew)
        dll_node_insert(head, _new, _afterNew);
    else
        dll_node_append(head, _new);
    return 0;
}

void dll_node_permut(aoc_ll_head_h head, struct dll_node *_a, struct dll_node *_b)
{
    struct dll_node *_anext = _a->_next;
    struct dll_node *_aprev = _a->_prev;

    if (head->_first == _a || head->_first == _b)
        head->_first = head->_first == _a ? _b : _a;

    if (head->_last == _a || head->_last == _b)
        head->_last = head->_last == _a ? _b : _a;

    _a->_prev = _b->_prev;
    _a->_next = _b->_next;

    if (_a->_prev)
        _a->_prev->_next = _a;
    if (_a->_next)
        _a->_next->_prev = _a;

    _b->_prev = _aprev;
    _b->_next = _anext;

    if (_b->_prev)
        _b->_prev->_next = _b;
    if (_b->_next)
        _b->_next->_prev = _b;
}

void dll_node_insert(aoc_ll_head_h head, struct dll_node *_a, struct dll_node *_b)
{
    assert(head && "Linked list head NULL");
    assert(_a && "Linked list _a is NULL");
    assert(_b && "Linked list _b is NULL");
    head->_size++;
    if (!_b->_prev)
        head->_first = _a;

    _a->_prev = _b->_prev;
    _a->_next = _b;
    if (_b->_prev)
        _b->_prev->_next = _a;
    _b->_prev = _a;
}

/*  Takes out node from the list without freeing its*/
void dll_node_disconnect(aoc_ll_head_h head, struct dll_node *_a)
{
    assert(head->_size != 0 && "attempt to remove link from an empty sinked list");
    assert(_a && "NULL pointer provided");

    head->_size--;
    if (_a->_prev)
        _a->_prev->_next = _a->_next;
    else
        head->_first = _a->_next;
    if (_a->_next)
        _a->_next->_prev = _a->_prev;
    else
        head->_last = _a->_prev;
    _a->_prev = NULL;
    _a->_next = NULL;
}

int dll_find_node(aoc_ll_head_h head, struct dll_node *_a)
{
    LL_FOREACH_P(_node, head)
    {
        if (!_node->_next)
            break;
        if (_node == _a)
            return 0;
    }
    return ENOENT;
}

size_t dll_count_nodes_by_property(aoc_ll_head_h head, void *_prop, bool (*equal)(void *_a, void *_b))
{
    size_t ret = 0;
    bool _found = false;
    if (!head || !equal)
        return 0;
    LL_FOREACH_P(_node, head)
    {
        _found = equal(_node, _prop);
        if (true == _found)
            ret++;
    }
    return ret;
}

struct dll_node *dll_find_node_by_property(aoc_ll_head_h head, void *_prop, bool (*equal)(void *_a, void *_b))
{
    bool _found = false;
    if (!head || !equal)
        return NULL;
    LL_FOREACH_P(_node, head)
    {
        _found = equal(_node, _prop);
        if (true == _found)
            return _node;
    }
    return NULL;
}

// TODO: finish this
void dll_sort(aoc_ll_head_h head, dll_compare(*comp))
{
    LL_FOREACH_P(_node, head)
    {
        if (!_node->_next)
            break;
        if (_node == comp(_node, _node->_next))
            dll_node_permut(head, _node, _node->_next);
    }
}

string_dll_node_h string_dll(const char *const name)
{
    string_dll_node_h _ret = malloc(sizeof(string_dll_node_t));
    if (!_ret)
        return NULL;
    _ret->_str = malloc(strnlen(name, MAX_STR_DLL_LEN) + 1);
    if (!_ret->_str)
        goto abort;

    strcpy(_ret->_str, name);
    return _ret;

abort:
    free(_ret);
    return NULL;
}

void string_dll_free(void *string)
{
    string_dll_node_h _stringh = (string_dll_node_h)string;
    free(_stringh->_str);
    free(_stringh);
}

struct dll_node *string_dll_compare(struct dll_node *_a, struct dll_node *_b)
{
    string_dll_node_h _string_a = (string_dll_node_h)_a;
    string_dll_node_h _string_b = (string_dll_node_h)_b;
    int _ret = strcmp(_string_a->_str, _string_b->_str);
    if (_ret >= 0)
        return _b;
    else
        return _a;
}

int string_dll_rename(string_dll_node_h node, const char *const name)
{
    char *_nname = malloc(strnlen(name, MAX_STR_DLL_LEN) + 1);
    if (!_nname)
        return ENOMEM;
    free(node->_str);
    sprintf(_nname, "%s", name);
    node->_str = _nname;
    return 0;
}
