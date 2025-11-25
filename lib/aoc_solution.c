#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "aoc_helpers.h"

void signal_int_handler(int sig){

}


int aocSolution(const char *_inPath, struct solutionCtrlBlock_t *_sol)
{
    int ret = 0;
    assert(_sol && "NULL solutionCtrlBlk provided by user");
    assert(_sol->_name && "NULL solution name provided by user");
    assert(_sol->_handler && "NULL _handler function provided by user");
    assert(_sol->_prologue && "NULL _prologue function provided by user");
    assert(_sol->_epilogue && "NULL _epilogue function provided by user");

    signal(SIGINT, signal_int_handler);

    FILE *_pxfile = fopen(_inPath, "r");
    if (_pxfile == NULL)
    {
        aoc_err("%s:%d: Error opening %s!", __func__, __LINE__, _inPath);
        exit(EXIT_FAILURE);
    }

    char _ins[MAX_LINE_LEN] = {0};

    aoc_info("Welcome to AOC 2022 %s", _sol->_name);
    if (_sol->_prologue(_sol))
        goto error;
    while (fgets(_ins, sizeof(_ins), _pxfile))
    {
        _sol->_str = _ins;
        ret = _sol->_handler(_sol);
        if (ret)
            goto error;
    }
    int epilogue = _sol->_epilogue(_sol);
    aoc_ans("AOC 2022 %s solution is %d", _sol->_name, epilogue);
    fclose(_pxfile);
    _sol->_free(_sol);
    return ret;
error:
    fclose(_pxfile);
    _sol->_free(_sol);
    aoc_err("AOC 2022 %s failed with error %d", _sol->_name, ret);
}
 