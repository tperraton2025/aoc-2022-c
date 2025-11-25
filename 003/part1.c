#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct letter_map_t
{
    char _arr[('z' - 'a') + ('Z' - 'A') + 2];
};

struct context_t
{
    struct letter_map_t _a;
    struct letter_map_t _b;
    int result;
};

#define CTX_CAST(_p) ((struct context_t *)_p)
#define TO_LTTR(_c) ((_c % ('z' - 'a' + 1)) + ((_c) > ('z' - 'a') ? 'A' : 'a'))

static void print_arr(struct letter_map_t *_map)
{
    char _out[sizeof(struct letter_map_t)];
    char *_pout = _out;
    for (size_t _c = 0; _c < sizeof(struct letter_map_t); _c++)
    {
        sprintf(_pout++, "%1u", _map->_arr[_c]);
    }
    aoc_info("%s", _out);
    _pout = _out;
    for (size_t _c = 0; _c < sizeof(struct letter_map_t); _c++)
    {
        sprintf(_pout++, "%c", (char)TO_LTTR(_c));
    }
    aoc_info("%s\n", _out);
}

static int prologue(struct solutionCtrlBlock_t *_blk)
{
    _blk->_data = malloc(sizeof(struct context_t));
    if (!_blk->_data)
        return ENOMEM;
    memset(_blk->_data, 0, sizeof(struct context_t));
    CTX_CAST(_blk->_data)->result = 0;
    return 0;
}

static void populate(struct letter_map_t *_map, const char *_str, const size_t _pos)
{
    for (size_t _c = 0; _c < _pos; _c++)
    {
        if (!isalpha(_str[_c]))
            continue;

        bool _is_upper = isupper(_str[_c]);
        size_t _decoded = _is_upper ? _str[_c] - 'A' : _str[_c] - 'a';
        char *_parr = _is_upper ? &_map->_arr[('z' - 'a') + 1] : &_map->_arr[0];
        _parr[_decoded] += 1;
    }
}

static char catchRepeat(struct solutionCtrlBlock_t *_blk)
{
    bool _caught = false;
    size_t _c = 0;
    for (_c = 0; _c < sizeof(struct letter_map_t); _c++)
    {
        if (CTX_CAST(_blk->_data)->_a._arr[_c] > 0 && CTX_CAST(_blk->_data)->_b._arr[_c] > 0)
        {
            _caught = true;
            break;
        }
    }
    assert(_caught && "false positive");
    return _c;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    memset(&CTX_CAST(_blk->_data)->_a, 0, sizeof(struct letter_map_t));
    memset(&CTX_CAST(_blk->_data)->_b, 0, sizeof(struct letter_map_t));

    size_t _len = strlen(_blk->_str) >> 1;
    populate(&CTX_CAST(_blk->_data)->_a, _blk->_str, _len);
    populate(&CTX_CAST(_blk->_data)->_b, _blk->_str + _len, _len);
    CTX_CAST(_blk->_data)->result += catchRepeat(_blk) + 1;
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    int result = CTX_CAST(_blk->_data)->result;
    aoc_ans("AOC 2022 %s solution is %d", _blk->_name, result);
    return result;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CAST(struct context_t *, _blk->_data);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;