
#define SLEEP_TIME_MS (0)
#include <stdlib.h>
#include <string.h>

typedef struct command_t
{
    struct ll_node_t _node;
    size_t count;
    size_t from;
    size_t to;
} command_t;

static command_t *command_ctor(command_t *_ncmd)
{
    command_t *ret = malloc(sizeof(struct command_t));
    if (!ret)
        return NULL;
    memset(ret, 0, sizeof(struct command_t));
    ret->count = _ncmd->count;
    ret->from = _ncmd->from;
    ret->to = _ncmd->to;
    return ret;
}

typedef struct aoc_2d_object_tracker_t
{
    struct ll_node_t _node;
    aoc_2d_object_t data;
    int _blocked;
} aoc_2d_object_tracker_t;

static aoc_2d_object_tracker_t *aoc_2d_object_tracker(aoc_2d_object_t _obj)
{
    aoc_2d_object_tracker_t *_ntracker = malloc(sizeof(aoc_2d_object_tracker_t));
    if (!_ntracker)
        return NULL;
    _ntracker->_node._prev = NULL;
    _ntracker->_node._next = NULL;
    _ntracker->_blocked = false;
    _ntracker->data = _obj;
    return _ntracker;
}

static void aoc_2d_object_tracker_free(void *arg)
{
    aoc_2d_object_tracker_t *_ntracker = CAST(aoc_2d_object_tracker_t *, arg);
    free(_ntracker);
}