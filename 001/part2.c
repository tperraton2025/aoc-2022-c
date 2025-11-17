#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct elfCal_t
{
    struct ll_node_t _node;
    char *_name;
    int _count;
    int _calories;
};

struct context_t
{
    struct ll_context_t _ll;
};

#define ELF_CAST(_p) ((struct elfCal_t *)_p)
#define CTX_CAST(_p) ((struct context_t *)_p->_data)

static struct elfCal_t *elf_ctor(struct context_t *_ctx)
{
    struct elfCal_t *_ret = malloc(sizeof(struct elfCal_t));

    if (ll_node_append(&_ctx->_ll, (struct ll_node_t *)_ret))
        goto cleanup;

    _ret->_count = _ctx->_ll._size;
    _ret->_calories = 0;
    _ret->_name = malloc(sizeof("elfo999"));
    if (!_ret->_name)
        goto cleanup;
    sprintf(_ret->_name, "elfo%3d", _ret->_count);
    return _ret;

cleanup:
    free(_ret);
    return _ret;
}

static void freeElf(void *_data)
{
    struct elfCal_t *_elf = ELF_CAST(_data);
    assert(_elf);
    assert(_elf->_name);
    free(_elf->_name);
    free(_elf);
}

static int prologue(struct solutionCtrlBlock_t *_blk)
{
    int ret = 0;
    _blk->_data = malloc(sizeof(struct context_t));
    struct context_t *_ctx = CAST(struct context_t*, _blk->_data);
    ll_blk_init(&_ctx->_ll);

    if (elf_ctor(_ctx))
        return 0;
    return 1;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CAST(struct context_t*, _blk->_data);

    int _cal = 0;
    if (sscanf(_blk->_str, "%d\n", &_cal))
        if (_cal)
        {
            ELF_CAST(_ctx->_ll._current)->_calories += _cal;
            LL_FOREACH(_node, _ctx->_ll)
            {
                if (ELF_CAST(_node)->_calories < ELF_CAST(_ctx->_ll._current)->_calories)
                {
                    aoc_ll_remove(&_ctx->_ll, _ctx->_ll._current);
                    aoc_ll_insert(&_ctx->_ll, _ctx->_ll._current, _node);
                    break;
                }
            }
        }
        else
        {
            if (elf_ctor(_ctx))
                return 0;
            else
                return 1;
        }
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    int result = 0, count = 0;
    struct context_t *_ctx = CAST(struct context_t*, _blk->_data);

    LL_FOREACH(_node, _ctx->_ll)
    {
        if (count++ < 3)
            result += ELF_CAST(_node)->_calories;
    }
    ll_free_all(&_ctx->_ll, freeElf);
    return result;
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue};
struct solutionCtrlBlock_t *part2 = &privPart2;