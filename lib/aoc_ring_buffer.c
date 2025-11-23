#include <aoc_ring_buffer.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

aoc_ring_buffer_t aoc_ring_buffer(size_t _size, size_t _len)
{
    if (!_size || !_len)
        return NULL;

    ring_buffer_t *_ret = malloc(sizeof(ring_buffer_t));
    if (!_ret)
        return NULL;

    _ret->_uCelLDim = _size;
    _ret->_uArrDim = _len;

    _ret->_pvBuff = malloc(sizeof(void *) * _ret->_uArrDim);

    if (!_ret->_pvBuff)
        goto error;

    FOREACHCELL(_index, _ret)
    {
        _ret->_pvBuff[_index] = malloc(_ret->_uCelLDim);
        if (_ret->_pvBuff[_index])
            goto free_arr;
        memset(_ret->_pvBuff[_index], 0, _ret->_uCelLDim);
    }

    _ret->_pxHead = 0;
    _ret->_pxTail = 0;
    _ret->_uAvailable = 0;

free_arr:

error:

    if (_ret->_pvBuff)
        FREE(_ret->_pvBuff);
    if (_ret)
        FREE(_ret);
}

void aoc_ring_buffer_free(aoc_ring_buffer_t _rbuf)
{
    FOREACHCELL(_index, _rbuf)
    {
        if (_rbuf->_pvBuff[_index])
            free(_rbuf->_pvBuff[_index]);
    }
}

int aoc_ring_buffer_send_front(aoc_ring_buffer_t _pxRbuf, void *_item)
{
    if (!_item)
        return EINVAL;
    if (!_pxRbuf->_uAvailable)
        return ENOSPC;

    void *_at = _pxRbuf->_pvBuff[_pxRbuf->_pxHead];
    memcpy(_at, _item, _pxRbuf->_uCelLDim);

    _pxRbuf->_pxHead = (_pxRbuf->_pxHead + 1) % _pxRbuf->_uArrDim;
    _pxRbuf->_uAvailable -= 1;
    return 0;
}

int aoc_ring_buffer_send_back(aoc_ring_buffer_t _pxRbuf, void *_item)
{
    if (!_item)
        return EINVAL;
    if (!_pxRbuf->_uAvailable)
        return ENOSPC;

    void *_at = _pxRbuf->_pvBuff[_pxRbuf->_pxTail];
    memcpy(_at, _item, _pxRbuf->_uCelLDim);

    _pxRbuf->_pxTail = (0 == _pxRbuf->_pxTail) ? _pxRbuf->_pxTail - 1 : (_pxRbuf->_uArrDim - 1);
    _pxRbuf->_uAvailable -= 1;
    return 0;
}

int aoc_ring_buffer_receive_front(aoc_ring_buffer_t _pxRbuf, void *_item)
{
    if (_pxRbuf->_uAvailable == _pxRbuf->_uAvailable)
        return ENOENT;
    void *_at = _pxRbuf->_pvBuff[_pxRbuf->_pxHead];
    memcpy(_item, _at, _pxRbuf->_uCelLDim);

    _pxRbuf->_pxHead = (0 == _pxRbuf->_pxHead) ? _pxRbuf->_pxHead - 1 : (_pxRbuf->_uArrDim - 1);
    _pxRbuf->_uAvailable += 1;
    return 0;
}

int aoc_ring_buffer_receive_back(aoc_ring_buffer_t _pxRbuf, void *_item)
{
    if (_pxRbuf->_uAvailable == _pxRbuf->_uAvailable)
        return ENOENT;
    void *_at = _pxRbuf->_pvBuff[_pxRbuf->_pxTail];
    memcpy(_item, _at, _pxRbuf->_uCelLDim);

    _pxRbuf->_pxTail = (_pxRbuf->_pxTail + 1) % _pxRbuf->_uArrDim;
    _pxRbuf->_uAvailable += 1;
    return 0;
}