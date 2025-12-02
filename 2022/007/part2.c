#include <aoc_helpers.h>
#include "partx.h"
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context
{
    dll_head_t _parsers;
    size_t _usedmem;
    fdir_t *_cdir;
    fdir_t *_root;
    size_t _memcheck;
    int result;
};

#define CTX_CAST(_p) ((struct context *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR, _blk->_name);
    _blk->_data = malloc(sizeof(struct context));
    struct context *_ctx = CTX_CAST(_blk->_data);

    _ctx->_root = dir("root", NULL);
    _ctx->_cdir = _ctx->_root;

    dll_head_init(&_ctx->_parsers);
    parser_append(&_ctx->_parsers, &cmdparser, _ctx);
    parser_append(&_ctx->_parsers, &fileparser, _ctx);
    parser_append(&_ctx->_parsers, &dirparser, _ctx);

    _ctx->_usedmem = 0;
    _ctx->result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    parse_all(&_ctx->_parsers, _blk->_str);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    get_mem(&_ctx->_root->path);

    size_t available_mem = 70000000 - _ctx->_root->size;
    size_t mem_to_free = available_mem < 30000000 ? 30000000 - available_mem : 0;
    filesearch_t _srch = {.threshold = mem_to_free, .totalMem = 0};

    aoc_tree_foreach_nodes_arg(&_ctx->_root->path, &_srch, get_smallest_dir_above_mem_lim);

    int result = _srch.totalMem;

    aoc_ans("AOC %s %s solution is %d", CONFIG_YEAR, _blk->_name, result);
    return result;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);
    aoc_tree_free(&_ctx->_root->path);
    FREE_AND_CLEAR_P(_ctx->_root);
    FREE_AND_CLEAR_P(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part2 = &privPart2;

static int parsecommand(void *arg, char *_str)
{
    aoc_context_h _ctx = (aoc_context_h)arg;
    int match = 0;
    char _cmd[MAX_NAME_LEN_AS_USIZE + 1] = {0};
    char _arg[MAX_NAME_LEN_AS_USIZE + 1] = {0};

    if (sscanf(_str, "$ %" MAX_NAME_LEN_AS_STR "s %" MAX_NAME_LEN_AS_STR "s", _cmd, _arg) > 0)
    {
        if (0 == strncmp("cd", _cmd, MAX_NAME_LEN_AS_USIZE))
        {
            aoc_tree_node_h _npwd = NULL;
            if (0 == strncmp("..", _arg, MAX_NAME_LEN_AS_USIZE))
            {
                _npwd = _ctx->_cdir->path._parent;
                if (!_npwd)
                    assert(0 == strcmp("root", _ctx->_cdir->name));
            }
            else if (0 == strncmp("/", _arg, MAX_NAME_LEN_AS_USIZE))
                _npwd = &_ctx->_root->path;
            else if (!_npwd)
            {
                LL_FOREACH(_subdir, _ctx->_cdir->path._dllchildren)
                {
                    _npwd = find_dir_by_name((aoc_tree_node_h)_subdir, _arg) ? (aoc_tree_node_h)_subdir : NULL;
                    if (_npwd)
                        break;
                }
            }
            if (_npwd)
                _ctx->_cdir = (fdir_t *)_npwd;
        }
        return 0;
    }
    return EINVAL;
}

static int parsefile(void *arg, char *_str)
{
    aoc_context_h _ctx = (aoc_context_h)arg;
    int match = 0;
    size_t _nfsize = 0;
    char _fullname[MAX_NAME_LEN_AS_USIZE] = {0};

    if (2 == sscanf(_str, "%ld %s", &_nfsize, _fullname))
    {
        char *_pname = _fullname;
        file_t *_nfile = file(_fullname, _nfsize, _ctx->_cdir);
        _ctx->_memcheck += _nfsize;
        if (_nfile)
            return 0;
        else
            return ENOMEM;
        return 0;
    }
    return EINVAL;
}

static int parsedir(void *arg, char *_str)
{
    aoc_context_h _ctx = (aoc_context_h)arg;
    int match = 0;
    char _dirname[16] = {0};

    if (1 == sscanf(_str, "dir %s", _dirname))
    {
        fdir_t *_ndir = dir(_dirname, _ctx->_cdir);
        if (!_ndir)
            aoc_err("failed to create %s", _dirname);
        return 0;
    }
    return EINVAL;
}
