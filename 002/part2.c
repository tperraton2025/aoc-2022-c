#include <aoc_helpers.h>
#include <aoc_ranges.h>
#include <aoc_linked_list.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context
{
    int result;
};

#define CTX_CAST(_p) ((struct context *)_p)

extern size_t score_ind_from_items(char mine, char his);

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    _blk->_data = malloc(sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    memset(_blk->_data, 0, sizeof(struct context));
    CTX_CAST(_blk->_data)->result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    char _outcome = 0;
    char _his = 0;
    char _mine = 0;

    const char *_psOutcome = NULL;
    const char *_psHis = NULL;
    const char *_psMine = NULL;

    int _luReqOutcomeInd = 0;
    int _luHisInd = 0;
    int _luMineInd = 0;

    const int _pI_points[] = {0, 3, 6};
    const char *const _psItemsStr[] = {"rock", "paper", "scissors"};
    const char *const _psOutcomeStr[] = {"I loose :(", "draw :|", "I win :)"};

    if (2 == sscanf(_blk->_str, "%c %c\n", &_his, &_outcome))
    {
        /* getting index from the items and outcome picked by players */
        _luHisInd = (int)(_his - 'A');
        _luReqOutcomeInd = (int)(_outcome - 'X');

        _luMineInd = _luHisInd + _luReqOutcomeInd - 1;
        if (_luMineInd < 0)
            _luMineInd = 3 + _luMineInd;
        else if (_luMineInd >= 3)
            _luMineInd = _luMineInd % 3;

        _mine = _luMineInd + 'A';

        _psOutcome = _psOutcomeStr[_luReqOutcomeInd];
        _psHis = _psItemsStr[_luHisInd];
        _psMine = _psItemsStr[_luMineInd];

        /* Depending on the outcome required */

        if (!N_IN_RANGE(_luHisInd, 0, 2) || !N_IN_RANGE(_luMineInd, 0, 2) || !N_IN_RANGE(_luReqOutcomeInd, 0, 2))
        {
            aoc_err("%s:%d  he picked:%i I picked:%i outcome:%i", __FILE__, __LINE__, _luHisInd,
                    _luMineInd,
                    _luReqOutcomeInd);
            return ERANGE;
        }

        size_t _indCheck = score_ind_from_items(_luMineInd, _luHisInd);
        if (_indCheck != _luReqOutcomeInd)
            aoc_info("he picked %-10s I picked %-10s -> %-12s and it should be %-12s", _psHis, _psMine, _psOutcomeStr[_indCheck], _psOutcome);

        int round = (_luMineInd + 1) + _pI_points[_luReqOutcomeInd];
        CTX_CAST(_blk->_data)->result += round;
    }
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
    struct context *_ctx = CAST(struct context *, _blk->_data);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part2 = &privPart2;