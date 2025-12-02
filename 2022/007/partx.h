#ifndef PART_X_H
#define PART_X_H
#include <string.h>
#include <stdlib.h>
#include <aoc_linked_list.h>
#include <aoc_parser.h>
#include <aoc_tree.h>

#define MAX_NAME_LEN_AS_USIZE (32)
#define MAX_NAME_LEN_AS_STR "32"

#define PART1_MEM_LIM (100000)

typedef struct
{
    struct tree_node path;
    struct dll_head _content;
    char *name;
    size_t size;
} fdir_t;

typedef fdir_t *fdir_h;
typedef struct
{
    struct dll_node path;
    fdir_t *dir;
    size_t size;
    char *name;
} file_t;

typedef fdir_t *file_h;
typedef struct
{
    struct dll_node _llnode;
    fdir_t *_fref;
} path_step_t;

typedef struct filesearch
{
    size_t totalMem;
    size_t threshold;
} filesearch_t;

typedef filesearch_t *filesearch_h;

static bool find_dir_by_name(void *_a, void *_b)
{
    if (!_a || !_b)
    {
        aoc_err("abnormal pointers provided in %s(%p,%p)", __func__, _a, _b);
        return false;
    }
    fdir_t *_pdir = (fdir_t *)_a;
    return (0 == strncmp(_pdir->name, (char *)_b, MAX_NAME_LEN_AS_USIZE) ? true : false);
}

static void pwd(fdir_t *_dir)
{
    struct dll_head path = {0};
    fdir_t *_srch = _dir;
    while (_srch)
    {
        path_step_t *_step = malloc(sizeof(path_step_t));
        _step->_fref = _srch;
        dll_node_append(&path, &_step->_llnode);
        _srch = (fdir_t *)_srch->path._parent;
    }
    LL_FOREACH_LAST(_llnode, path)
    {
        printf("%s/", ((path_step_t *)_llnode)->_fref->name);
    }
    printf("\t"
           "%ld"
           "\n",
           _dir->size);
    dll_free_all(&path, free);
}

static size_t get_mem(aoc_tree_node_h _dir)
{
    size_t _mem = 0;
    fdir_t *_fsp = (fdir_t *)_dir;
    LL_FOREACH(_dllfiles, _fsp->_content)
    {
        file_t *_f = (file_t *)_dllfiles;
        _mem += _f->size;
    }
    LL_FOREACH(_dllsubdirs, _fsp->path._dllchildren)
    {
        fdir_t *_d = (fdir_t *)_dllsubdirs;
        _mem += get_mem(&_d->path);
    }
    _fsp->size = _mem;
    return _mem;
}

static fdir_t *cd(fdir_h _fsp, char *_str)
{
    if (!_fsp || !_str)
        return _fsp;

    aoc_tree_node_h _tnode = (aoc_tree_node_h)_fsp;
    fdir_t *_npwd = NULL;
    if (0 == strncmp("..", _str, MAX_NAME_LEN_AS_USIZE))
        _npwd = (fdir_t *)_tnode->_parent;
    if (0 == strncmp("/", _str, MAX_NAME_LEN_AS_USIZE))
        _npwd = (fdir_t *)aoc_tree_find_root(_tnode);
    if (!_npwd)
    {
        LL_FOREACH(_node, _tnode->_dllchildren)
        {
            aoc_tree_node_h _stnode = (aoc_tree_node_h)_node;
            _npwd = (fdir_t *)(find_dir_by_name(_stnode, _str) ? _stnode : NULL);
            if (_npwd)
                break;
        }
    }
    if (_npwd)
    {
        // pwd(_npwd);
        return _npwd;
    }
    // aoc_info("dir %s not found in %s", _str, _fsp->name);
    return _fsp;
}

static struct dll_node *uglyls_compare(struct dll_node *_a, struct dll_node *_b)
{
    string_dll_node_h _string_a = (string_dll_node_h)_a;
    string_dll_node_h _string_b = (string_dll_node_h)_b;

    char *_a_str = strchr(_string_a->_str, ' ');
    char *_b_str = strchr(_string_b->_str, ' ');
    int _ret = strcmp(_a_str, _b_str);
    if (_ret >= 0)
        return _b;
    else
        return _a;
}

static void uglyls(aoc_tree_node_h _dir)
{
    struct dll_head namelist = {0};
    fdir_t *_fsp = (fdir_t *)_dir;

    LL_FOREACH(_dllfiles, _fsp->_content)
    {
        file_t *_f = (file_t *)_dllfiles;
        char *_fname = malloc(strlen("0000000000000000000") + strlen(_f->name) + 1);
        sprintf(_fname, "%ld %s", _f->size, _f->name);
        string_dll_node_h _nName = string_dll(_fname);
        dll_node_sorted_insert(&namelist, &_nName->_node, uglyls_compare);
        FREE_AND_CLEAR_P(_fname);
    }
    LL_FOREACH(_dllsubdirs, _fsp->path._dllchildren)
    {
        fdir_t *_d = (fdir_t *)_dllsubdirs;
        char *_dname = malloc(strlen("dir ") + strlen(_d->name) + 1);
        sprintf(_dname, "dir %s", _d->name);
        string_dll_node_h _nName = string_dll(_dname);
        dll_node_sorted_insert(&namelist, &_nName->_node, uglyls_compare);
        FREE_AND_CLEAR_P(_dname);
    }
    LL_FOREACH(_sdll, namelist)
    {
        printf("%s\n", ((string_dll_node_t *)_sdll)->_str);
    }
    dll_free_all(&namelist, string_dll_free);
}

static void ls(aoc_tree_node_h _dir)
{
    fdir_t *_fsp = (fdir_t *)_dir;
    pwd(_fsp);
    LL_FOREACH(_dllfiles, _fsp->_content)
    {
        file_t *_f = (file_t *)_dllfiles;
        printf(GREEN "\tfile %s" RESET RED " %ld" RESET "\n", _f->name, _f->size);
    }
    LL_FOREACH(_dllsubdirs, _fsp->path._dllchildren)
    {
        fdir_t *_d = (fdir_t *)_dllsubdirs;
        printf(YELLOW "\tdir %s" RESET "\n", _d->name);
    }
}

static int file_exists_in_cdir(file_t *nfile, fdir_t *cdir)
{
    LL_FOREACH(_dllfiles, cdir->_content)
    {
        file_t *_f = (file_t *)_dllfiles;
        if ((0 == strcmp(nfile->name, _f->name)) && (_f->dir == nfile->dir))
        {
            aoc_err("%s exists in %s", nfile->name, cdir->name);
            return EEXIST;
        }
    }
    return 0;
}

static int dir_exists_in_cdir(fdir_h fdir, fdir_h cdir)
{
    assert(fdir && "NULL handle provided");
    if (cdir)
    {
        LL_FOREACH(_dlldirs, cdir->path._dllchildren)
        {
            fdir_h _dir = (fdir_h)_dlldirs;
            if ((0 == strcmp(fdir->name, _dir->name)) && (_dir->path._parent == fdir->path._parent))
            {
                aoc_err("%s exists in %s", fdir->name, cdir->name);
                return EEXIST;
            }
        }
    }
    return 0;
}

static void delete_file(void *arg)
{
    file_t *_file = (file_t *)arg;
    FREE_AND_CLEAR_P(_file->name);
    FREE_AND_CLEAR_P(_file);
}
static void delete_dir(void *arg)
{
    fdir_t *_dir = (fdir_t *)arg;
    dll_free_all(&_dir->_content, delete_file);
    FREE_AND_CLEAR_P(_dir->name);
}

static fdir_t *dir(const char *const name, fdir_t *parent)
{
    fdir_t *_ndir = malloc(sizeof(fdir_t));
    if (!_ndir)
        return NULL;

    memset(_ndir, 0, sizeof(fdir_t));
    _ndir->name = malloc(strnlen(name, MAX_NAME_LEN_AS_USIZE) + 1);
    if (!_ndir->name)
        goto abort_dir;
    sprintf(_ndir->name, "%s", name);
    int ret = dir_exists_in_cdir(_ndir, parent);

    if (ret)
        goto abort;

    aoc_tree_node(parent ? &parent->path : NULL, &_ndir->path, delete_dir);
    _ndir->path._parent = parent ? &parent->path : NULL;
    return _ndir;

abort:
    aoc_err("aborting %s creation: %s", name, strerror(ret));
    FREE_AND_CLEAR_P(_ndir->name);
abort_dir:
    FREE_AND_CLEAR_P(_ndir);
    return _ndir;
}
static file_t *file(char *name, size_t size, fdir_t *dir)
{
    if (!name || !size || !dir)
        return NULL;
    file_t *_nfile = malloc(sizeof(file_t));
    if (!_nfile)
        return NULL;
    _nfile->name = malloc(strnlen(name, MAX_NAME_LEN_AS_USIZE) + 1);
    if (!_nfile->name)
    {
        FREE_AND_CLEAR_P(_nfile);
        return NULL;
    }
    sprintf(_nfile->name, "%s", name);
    _nfile->size = size;
    _nfile->dir = dir;

    if (file_exists_in_cdir(_nfile, dir))
        goto abort;

    if (dll_node_append(&dir->_content, &_nfile->path))
        goto abort;

    return _nfile;

abort:
    delete_file(_nfile);
    _nfile = NULL;
    return _nfile;
}
static void sum_files_above_mem_lim(void *arg, aoc_tree_node_h _a)
{
    filesearch_t *_tot = (filesearch_t *)arg;
    fdir_t *_fsp = (fdir_t *)_a;

    LL_FOREACH(_dllfiles, _fsp->_content)
    {
        file_t *_f = (file_t *)_dllfiles;
        // if (_f->size > PART1_MEM_LIM)
        _tot->totalMem += _f->size;
    }
}
static void sum_dirs_below_mem_lim(void *arg, aoc_tree_node_h _a)
{
    filesearch_t *_tot = (filesearch_t *)arg;
    fdir_t *_fsp = (fdir_t *)_a;

    LL_FOREACH(_dllsubdirs, _fsp->path._dllchildren)
    {
        fdir_t *_d = (fdir_t *)_dllsubdirs;
        if (_d->size <= PART1_MEM_LIM)
            _tot->totalMem += _d->size;
    }
}
static void get_smallest_dir_above_mem_lim(void *arg, aoc_tree_node_h _a)
{
    filesearch_t *_tot = (filesearch_t *)arg;
    fdir_t *_fsp = (fdir_t *)_a;

    LL_FOREACH(_dllsubdirs, _fsp->path._dllchildren)
    {
        fdir_t *_d = (fdir_t *)_dllsubdirs;
        if (_d->size > _tot->threshold)
        {
            if ((!_tot->totalMem) || (_tot->totalMem > _d->size))
                _tot->totalMem = _d->size;
        }
    }
}
 
static int command_executor(aoc_context_h _ctx, char *_str);

static int parsecommand(void *arg, char *_str);
static int parsefile(void *arg, char *_str);
static int parsedir(void *arg, char *_str);

static struct parser cmdparser = {._name = "cmd parser", ._func = parsecommand};
static struct parser fileparser = {._name = "file parser", ._func = parsefile};
static struct parser dirparser = {._name = "dir parser", ._func = parsedir};
 
#endif
