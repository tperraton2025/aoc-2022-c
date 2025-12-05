#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <aoc_ranges.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context
{
    int result;
};

static char mine;
static char his;
static int points[] = {0, 3, 6};
static char *names[] = {"rock", "paper", "scissors"};

static size_t matrix[3][3] = {{1, 0, 2}, {2, 1, 0}, {0, 2, 1}};

#define CTX_CAST(_p) ((struct context *)_p)

size_t score_ind_from_items(size_t mine, size_t his)
{
    return matrix[mine][his];
}

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR,  _blk->_name);
    TRY_RAII_MALLOC(_blk->_data, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM; 
    CTX_CAST(_blk->_data)->result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{

    if (2 == sscanf(_blk->_str, "%c %c\n", &his, &mine))
    {
        his -= 'A';
        mine -= 'X';
        size_t index = score_ind_from_items(mine, his);
        if (!N_BETWEEN_AB(his, 0, 2) || !N_BETWEEN_AB(mine, 0, 2) || !N_BETWEEN_AB(index, 0, 2))
        {
            aoc_err("%s:%d his:%u mine:%u index:%lu", __FILE__, __LINE__, (uint8_t)his, (uint8_t)mine, index);
            exit(EXIT_FAILURE);
        }
        int round = (mine + 1) + points[index];
        CTX_CAST(_blk->_data)->result += round;
    }
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    int result = CTX_CAST(_blk->_data)->result;
    aoc_ans("AOC %s %s solution is %d", CONFIG_YEAR, _blk->_name, result);
    return result;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);
    FREE(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;