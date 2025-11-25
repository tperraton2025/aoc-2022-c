#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <aoc_input.h>
#include <aoc_iterator.h>
#include <aoc_threads_helpers.h>
#include <aoc_ring_buffer.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/stat.h>
#include <pthread.h>
#include <mqueue.h>
#include <signal.h>

typedef enum
{
    QUEUE_MSG_INPUT = 0,
    QUEUE_MSG_KILL,
    QUEUE_MSG_MAX
} queue_msg_type_enum_t;

#define MQ_MSG_MAX_SIZE (32)

typedef struct
{
    queue_msg_type_enum_t _eType;
    void *_vData;
} queue_msg_t;

typedef struct
{
    int _ret;
    void *_arg;
} thead_data_t;

typedef struct
{
    mqd_t _iMqueue;
    char *_sName;
    struct mq_attr _attr;
    mode_t _xMode;
} _hQueue_t;

typedef struct
{
    pthread_t _thr;
    _hQueue_t _xQueue;
    aoc_2d_engine_t _eng;
    aoc_2d_object_t _cur;
    struct ll_context_t _objects_ll;
    int _iRet;
} graphics_t;

typedef struct
{
    pthread_t _thr;
    _hQueue_t _xQueue;
    aoc_ring_buffer_t _pxInputCharBuffer;
    struct ll_context_t _inputs_ll;
    int _iRet;
} inputs_t;

struct context_t
{
    graphics_t _graphics;
    inputs_t _inputs;
    _hQueue_t *_hMqueues[8];
    size_t _hMq_top;
    int result;
};

#define CTX_CAST(_p) ((struct context_t *)_p)
#define GRP_CAST(_p) ((graphics_t *)_p)
#define INP_CAST(_p) ((inputs_t *)_p)
#define THR_CAST(_p) ((thead_data_t *)_p)

#define MOV_CAST(_p) ((movement_t *)_p)
#define MAP_MID_SIZE (16)

#define MQ_NAME_SIZE_UINT (8)
#define MQ_NAME_SIZE_FMT "16"

static struct solutionCtrlBlock_t privPart1;
void *graphics_routine(void *args);
void *inputs_routine(void *args);

void sig_int_handler(int args)
{
    struct context_t *_ctx = CTX_CAST(privPart1._data);
    aoc_warn("received signal %s", strsignal(args));

    if (SIGINT | SIGKILL | SIGABRT | SIGSTOP | SIGTSTP & args)
    {
        FOREACH_P_NONNULL(_ii, _ctx->_hMqueues)
        {
            _hQueue_t *_hmq = _ctx->_hMqueues[_ii];
            queue_msg_t _xIntMsg = {._eType = QUEUE_MSG_KILL, ._vData = NULL};
            char *_pcIntMsg = (char *)&_xIntMsg;

            if (_hmq->_iMqueue)
            {
                int ret = mq_send(_hmq->_iMqueue, _pcIntMsg, sizeof(queue_msg_t), 1);
                if (ret)
                    aoc_warn("%s: mq_send %i: %s", _hmq->_sName, ret, strerror(ret));
            }
        }
    }
}

void queue_setup(struct context_t *_ctx, _hQueue_t *_pxQueue, const char *_name)
{
    size_t len = strnlen(_name, MQ_NAME_SIZE_UINT);
    _pxQueue->_sName = malloc(len + 1);
    snprintf(_pxQueue->_sName, MQ_NAME_SIZE_UINT, "%s", _name);
    printf("%s\n", _pxQueue->_sName);
    _pxQueue->_xMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    _pxQueue->_attr.mq_curmsgs = 0;
    _pxQueue->_attr.mq_flags = 0;
    _pxQueue->_attr.mq_maxmsg = 10;
    _pxQueue->_attr.mq_msgsize = sizeof(queue_msg_t);

    _ctx->_hMqueues[_ctx->_hMq_top++] = _pxQueue;
}

static int prologue(struct solutionCtrlBlock_t *_blk)
{
    int ret = 1;
    _blk->_data = malloc(sizeof(struct context_t));
    if (!_blk->_data)
        return ENOMEM;
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    memset(_ctx, 0, sizeof(struct context_t));

    _ctx->result = 0;

    queue_setup(_ctx, &_ctx->_graphics._xQueue, "/grap_mq");
    queue_setup(_ctx, &_ctx->_inputs._xQueue, "/input_mq");

    pthread_create(&_ctx->_graphics._thr, NULL, graphics_routine, &_ctx->_graphics);
    pthread_create(&_ctx->_inputs._thr, NULL, inputs_routine, &_ctx->_inputs);

    signal(SIGINT, sig_int_handler);
    signal(SIGKILL, sig_int_handler);
    signal(SIGABRT, sig_int_handler);
    signal(SIGSTOP, sig_int_handler);
    signal(SIGTSTP, sig_int_handler);

    pthread_join(_ctx->_graphics._thr, NULL);
    pthread_join(_ctx->_inputs._thr, NULL);

    free(_ctx->_graphics._xQueue._sName);
    free(_ctx->_inputs._xQueue._sName);
    mq_close(_ctx->_graphics._xQueue._iMqueue);
    mq_close(_ctx->_inputs._xQueue._iMqueue);
    return 0;

error:
    free(_blk->_data);
    return ret;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context_t *_ctx = CTX_CAST(_blk->_data);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    free(_blk->_data);
    return 0;
}

void *graphics_routine(void *args)
{
    graphics_t *_grp = GRP_CAST(args);

    coord_t resolution = {._x = 1, ._y = 1};
    coord_t spaceing = {._x = 1, ._y = 1};

    ALLOCATE_AND_RETURN_IF_NULL(_grp->_eng, engine_create(resolution, spaceing, '.'),
                                _grp->_iRet,
                                ENOMEM,
                                exit);

    ll_blk_init(&_grp->_objects_ll);

    aoc_engine_resize_one_direction(_grp->_eng, MAP_MID_SIZE, AOC_DIR_RIGHT);
    aoc_engine_resize_one_direction(_grp->_eng, MAP_MID_SIZE, AOC_DIR_LEFT);
    aoc_engine_resize_one_direction(_grp->_eng, MAP_MID_SIZE, AOC_DIR_UP);
    aoc_engine_resize_one_direction(_grp->_eng, MAP_MID_SIZE, AOC_DIR_DOWN);

    ALLOCATE_AND_RETURN_IF_NULL(_grp->_cur,
                                eng_obj_create(_grp->_eng, "cursor", NULL, "H", OBJ_PROPERTY_NO_COLLISION | OBJ_PROPERTY_MOBILE),
                                _grp->_iRet,
                                ENOMEM,
                                exit);

    ASSIGN_AND_RETURN_IF_NOT_0(_grp->_iRet,
                               aoc_engine_append_obj(_grp->_eng, _grp->_cur),
                               free_object);

    _hQueue_t *_hMq = &_grp->_xQueue;

    _hMq->_iMqueue = mq_open(_hMq->_sName, O_RDWR | O_NONBLOCK | O_CREAT, _hMq->_xMode, &_hMq->_attr);
    if (0 > _hMq->_iMqueue)
        goto exit;

    engine_deactivate_drawing(_grp->_eng);
    // engine_draw(_grp->_eng);
    usleep(100 * 1000);

    char _cMsgQueueBuffer[sizeof(queue_msg_t) + 1] = {0};
    int mq_ret = mq_receive(_hMq->_iMqueue, _cMsgQueueBuffer, sizeof(_cMsgQueueBuffer), NULL);
    while (!_cMsgQueueBuffer[0])
    {
        usleep(100 * 1000);
        _cMsgQueueBuffer[0] = 'c';
        mq_ret = mq_receive(_hMq->_iMqueue, _cMsgQueueBuffer, sizeof(_cMsgQueueBuffer), NULL);
    }
    goto exit;

free_object:
    if (_grp->_cur)
        eng_obj_free(_grp->_cur);
exit:
    mq_unlink(_hMq->_sName);
    if (_grp->_eng)
        engine_free(_grp->_eng);

    aoc_info("%s exited normally" RESET, __func__);
    pthread_exit(NULL);
}

void *inputs_routine(void *args)
{
    inputs_t *_inp = INP_CAST(args);

    fcntl(stdin->_fileno, F_SETFL, O_NONBLOCK);

    char _cByte = ' ';
    _inp->_pxInputCharBuffer = aoc_ring_buffer(sizeof(char), 64);
    _hQueue_t *_hMq = &_inp->_xQueue;

    _hMq->_iMqueue = mq_open(_hMq->_sName, O_RDWR | O_NONBLOCK | O_CREAT, _hMq->_xMode, &_hMq->_attr);
    if (0 > _hMq->_iMqueue)
        goto error;

    usleep(100 * 1000);

    char _cMsgQueueBuffer[sizeof(queue_msg_t) + 1] = {0};
    int mq_ret = mq_receive(_hMq->_iMqueue, _cMsgQueueBuffer, sizeof(_cMsgQueueBuffer), NULL);

    bool _bMustQuit = false;
    while (!_cMsgQueueBuffer[0])
    {
        usleep(10 * 1000);
        char c[16] = {0};
        _cMsgQueueBuffer[0] = 'c';
        char cret = 0;
        read(stdin->_fileno, &cret, 1);
        if (cret)
        {
            printf(RED "%c" RESET, cret);
            cret = 0;
        }
        mq_ret = mq_receive(_hMq->_iMqueue, _cMsgQueueBuffer, sizeof(_cMsgQueueBuffer), NULL);
    }

error:
    if (0 < _hMq->_iMqueue)
        mq_unlink(_hMq->_sName);
    if (_inp->_pxInputCharBuffer)
        aoc_ring_buffer_free(_inp->_pxInputCharBuffer);

    aoc_info("%s exited normally" RESET, __func__);
    pthread_exit(NULL);
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{ 
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;
