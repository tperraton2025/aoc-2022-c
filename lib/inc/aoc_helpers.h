#ifndef AOC_HELPERS
#define AOC_HELPERS

#include <stdio.h>
#include "ansi_fmt.h"
#include "aoc_types.h"
#include <assert.h>
#include <unistd.h>
#include <errno.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


#define FREE(_p) \
    {                        \
        if (_p)              \
            free(_p);        \
        _p = NULL;           \
    }

#define TRY_RAII_MALLOC(_p, _s) \
    {                           \
        _p = malloc(_s);        \
        if (_p)                 \
            memset(_p, 0, _s);  \
    }

#define ARRAY_DIM(_arr) (sizeof(_arr) / sizeof(_arr[0]))
#define CAST(_t, _p) ((_t)_p)
#define MAX_LINE_LEN (1024)

#define ARR_FOREACH(_it, _arr) for (size_t _it = 0; _it < ARRAY_DIM(_arr); _it++)

#define aoc_info(_fmt, ...) printf(_fmt "\r\n", __VA_ARGS__);
#define aoc_err(_fmt, ...) printf(RED _fmt RESET "\r\n", __VA_ARGS__);
#define aoc_warn(_fmt, ...) printf(YELLOW _fmt RESET "\r\n", __VA_ARGS__);
#define aoc_ans(_fmt, ...) printf(UGREEN _fmt RESET "\r\n", __VA_ARGS__);

#define block()                                               \
    {                                                         \
        aoc_info("%s %s:%i", "blocked", __FILE__, __LINE__); \
        scanf("%*c");                                         \
    }

struct solutionCtrlBlock_t
{
    char *_name;
    char *_str;
    int (*_prologue)(struct solutionCtrlBlock_t *_data, int argc, char *argv[]);
    int (*_handler)(struct solutionCtrlBlock_t *_data);
    int (*_epilogue)(struct solutionCtrlBlock_t *_data);
    void (*_free)(struct solutionCtrlBlock_t *_data);
    void *_data;
};

/**
 * Takes path to input txt file and ensures its validity
 * It then handles solving the challenge by calling _func
 * with each line of the file successfully opened until
 * it reaches its end;
 *
 * @param _inPath   : path to file
 * @param _func     : int returning function pointer
 */
extern int aocSolution(struct solutionCtrlBlock_t *_sol, int argc, char *argv[]);

#endif