#include <aoc_helpers.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

extern struct solutionCtrlBlock_t *part1;
extern struct solutionCtrlBlock_t *part2;

int main(int argc, char *argv[])
{
    assert(argc > 1 && "Wrong number of arguments provided");
    aocSolution(part1, argc, argv);
    aocSolution(part2, argc, argv);
    return 0;
}