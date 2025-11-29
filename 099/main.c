#include <aoc_helpers.h>
#include <unistd.h>

extern struct solutionCtrlBlock_t *part1;

int main(int argc, char *argv[])
{
    assert(argc > 1 && "Wrong number of arguments provided");
    aocSolution(part1, argc, argv);
    return 0;
}