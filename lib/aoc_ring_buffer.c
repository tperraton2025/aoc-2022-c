#include <aoc_ring_buffer.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct ring_buffer
{
    //   aoc_linked_list_t _items;
    void **_pv_buff;
    size_t _px_head;
    size_t _px_tail;
    size_t _u_available;
    size_t _u_cell_dim;
    size_t _u_arr_dim;
};

aoc_ring_buffer_h aoc_ring_buffer(size_t _size, size_t _len)
{
    if (!_size || !_len)
        return NULL;

    struct ring_buffer *_ret = malloc(sizeof(struct ring_buffer));
    if (!_ret)
        return NULL;

    _ret->_u_cell_dim = _size;
    _ret->_u_arr_dim = _len;

    _ret->_pv_buff = malloc(sizeof(void *) * _ret->_u_arr_dim);

    if (!_ret->_pv_buff)
        goto error;

    FOREACHCELL(_index, _ret)
    {
        _ret->_pv_buff[_index] = malloc(_ret->_u_cell_dim);
        if (!_ret->_pv_buff[_index])
            goto free_arr;
        memset(_ret->_pv_buff[_index], 0, _ret->_u_cell_dim);
    }

    _ret->_px_head = 0;
    _ret->_px_tail = 0;
    _ret->_u_available = _len;

    return _ret;
free_arr:

error:

    if (_ret->_pv_buff)
        FREE(_ret->_pv_buff);
    if (_ret)
        FREE(_ret);
}

void aoc_ring_buffer_free(aoc_ring_buffer_h _pxRbuf)
{
    assert(_pxRbuf && "NULL pointer provided");
    FOREACHCELL(_index, _pxRbuf)
    {
        if (_pxRbuf->_pv_buff[_index])
            free(_pxRbuf->_pv_buff[_index]);
    }
    free(_pxRbuf->_pv_buff);
    free(_pxRbuf);
}

int aoc_ring_buffer_send_front(aoc_ring_buffer_h _pxRbuf, void *_item)
{
    assert(_pxRbuf && "NULL pointer provided");
    if (!_item)
        return EINVAL;
    if (!_pxRbuf->_u_available)
        return ENOSPC;

    void *_at = _pxRbuf->_pv_buff[_pxRbuf->_px_head];
    memcpy(_at, _item, _pxRbuf->_u_cell_dim);

    _pxRbuf->_px_head = (_pxRbuf->_px_head + 1) % _pxRbuf->_u_arr_dim;
    _pxRbuf->_u_available -= 1;
    return 0;
}

int aoc_ring_buffer_send_back(aoc_ring_buffer_h _pxRbuf, void *_item)
{
    assert(_pxRbuf && "NULL pointer provided");

    if (!_item)
        return EINVAL;
    if (!_pxRbuf->_u_available)
        return ENOSPC;

    void *_at = _pxRbuf->_pv_buff[_pxRbuf->_px_tail];
    memcpy(_at, _item, _pxRbuf->_u_cell_dim);

    _pxRbuf->_px_tail = (0 == _pxRbuf->_px_tail) ? _pxRbuf->_px_tail - 1 : (_pxRbuf->_u_arr_dim - 1);
    _pxRbuf->_u_available -= 1;
    return 0;
}

int aoc_ring_buffer_receive_front(aoc_ring_buffer_h _pxRbuf, void *_item)
{
    assert(_pxRbuf && "NULL pointer provided");
    assert(_item && "NULL pointer provided");
    if (_pxRbuf->_u_available == _pxRbuf->_u_available)
        return ENOENT;
    void *_at = _pxRbuf->_pv_buff[_pxRbuf->_px_head];
    memcpy(_item, _at, _pxRbuf->_u_cell_dim);

    _pxRbuf->_px_head = (0 == _pxRbuf->_px_head) ? _pxRbuf->_px_head - 1 : (_pxRbuf->_u_arr_dim - 1);
    _pxRbuf->_u_available += 1;
    return 0;
}

int aoc_ring_buffer_receive_back(aoc_ring_buffer_h _pxRbuf, void *_item)
{
    assert(_pxRbuf && "NULL pointer provided");
    if (_pxRbuf->_u_available == _pxRbuf->_u_arr_dim)
        return ENOENT;
    void *_at = _pxRbuf->_pv_buff[_pxRbuf->_px_tail];
    memcpy(_item, _at, _pxRbuf->_u_cell_dim);

    _pxRbuf->_px_tail = (_pxRbuf->_px_tail + 1) % _pxRbuf->_u_arr_dim;
    _pxRbuf->_u_available += 1;
    return 0;
}

size_t aoc_ring_buffer_available(aoc_ring_buffer_h _rbuf)
{
    assert(_rbuf && "NULL pointer provided");
    struct ring_buffer *_rb = (struct ring_buffer *)_rbuf;
    return _rb->_u_available;
}

size_t aoc_ring_buffer_item_size(aoc_ring_buffer_h _rbuf)
{
    assert(_rbuf && "NULL pointer provided");
    struct ring_buffer *_rb = (struct ring_buffer *)_rbuf;
    return _rb->_u_cell_dim;
}