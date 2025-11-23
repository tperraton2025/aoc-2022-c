#include <aoc_stack.h>
#include <errno.h>

struct aoc_stack_t
{
    size_t _size;
    size_t _count;
    void *_top;
    void **_base;
    void (*_free)(void *args);
};

#define STK_CAST(_p) ((struct aoc_stack_t *)_p)

size_t aoc_stack_count(aoc_stack_handle_t _stkh) { return _stkh->_count; }

aoc_stack_handle_t aoc_stack_init(void (*_free)(void *args), size_t _size)
{
    if (!_free || !_size)
        return NULL;

    struct aoc_stack_t *_ret = malloc(sizeof(struct aoc_stack_t));
    if (!_ret)
        return NULL;
    _ret->_base = NULL;
    _ret->_count = 0;
    _ret->_size = _size;
    _ret->_top = NULL;

    _ret->_free = _free;
    return _ret;
}

int aoc_stack_push(aoc_stack_handle_t _stkh, void *_new)
{
    if (!_stkh || !_new)
        return EINVAL;

    struct aoc_stack_t *_stk = STK_CAST(_stkh);
    _stk->_count++;

    if (_stk->_base)
        _stk->_base = realloc(_stk->_base, _stk->_size * _stk->_count);
    else
        _stk->_base = malloc(_stk->_size * _stk->_count);

    if (!_stk->_base)
        return ENOMEM;

    _stk->_top = _stk->_base + (_stk->_count * _stk->_size);
    memcpy(_stk->_top, _new, _stk->_size);
    return 0;
}

int aoc_stack_pop(aoc_stack_handle_t _stkh, void *_item)
{
    struct aoc_stack_t *_stk = STK_CAST(_stkh);

    if (!_stkh)
        return EINVAL;

    if (!_stk->_base || !_stk->_top)
        return EINVAL;

    if (_item)
        memcpy(_item, _stk->_top, _stk->_size);

    _stk->_free(_stk->_top);

    _stk->_count--;
    if (_stk->_base)
        _stk->_base = realloc(_stk->_base, _stk->_size * _stk->_count);

    if (!_stk->_base)
        return ENOMEM;

    _stk->_top = _stk->_base + (_stk->_count * _stk->_size);
    return 0;
}

void aoc_stack_free(void *_stkh)
{
    struct aoc_stack_t *_stk = STK_CAST(_stkh);

    if (!_stkh)
        return;

    if (!_stk->_top || !_stk->_base)
        return;

    for (size_t _ii = _stk->_count; _ii; _ii--)
        aoc_stack_pop(_stk, NULL);

    free(_stk->_base);
    free(_stk);
}