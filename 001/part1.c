#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct elfCal_t
{
    LL_DECLARE(elfCal_t)
    char *_name;
    int _count;
    int _calories;
};

struct data_t
{
    struct elfCal_t *_first;
    struct elfCal_t *_current;
    int _maxCalories;
};

LL_CTOR(elfCal_t, elfNodeCtor);

static struct elfCal_t *elfCtor(struct elfCal_t *_prev)
{
    struct elfCal_t *_ret = elfNodeCtor(_prev);
    _ret->_count = _prev ? _prev->_count + 1 : 0;
    _ret->_calories = 0;
    _ret->_name = malloc(sizeof("elfo 999"));
    sprintf(_ret->_name, "elfo%3d", _ret->_count);
    return _ret;
}

static void freeElf(struct elfCal_t *_elf)
{
    assert(_elf);
    assert(_elf->_name);
    FREE(_elf->_name);
    FREE(_elf);
}

static void freeAllElves(struct elfCal_t *_first)
{
    if (!_first)
        return;
    struct elfCal_t *_elf = _first;
    struct elfCal_t *_next = _elf->_prev;
    assert(_elf);
    assert(_next);
    while (_elf)
    {
        _next = _elf->_prev;
        freeElf(_elf);
        _elf = _next;
    }
}

static int prologue(struct solutionCtrlBlock_t *_blk)
{
    _blk->_data = malloc(sizeof(struct data_t));
    struct data_t *_pd = CAST(struct data_t *, _blk->_data);
    _pd->_first = elfCtor(NULL);
    _pd->_current = _pd->_first;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct data_t *_pdata = CAST(struct data_t *, _blk->_data);
    struct elfCal_t *_p_last_elf = CAST(struct elfCal_t *, _pdata->_current);

    int _cal = 0;
    if (sscanf(_blk->_str, "%d\n", &_cal))
        if (_cal)
        {
            _p_last_elf->_calories += _cal;
            _pdata->_maxCalories = _p_last_elf->_calories > _pdata->_maxCalories ? _p_last_elf->_calories : _pdata->_maxCalories;
        }
        else
        {
            _p_last_elf->_next = elfCtor(_p_last_elf);
            _pdata->_current = _p_last_elf->_next;
        }
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct data_t *_data = CAST(struct data_t *, _blk->_data);
    int result = _data->_maxCalories;
    freeAllElves(_data->_current);
    return result;
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue};
struct solutionCtrlBlock_t *part1 = &privPart1;