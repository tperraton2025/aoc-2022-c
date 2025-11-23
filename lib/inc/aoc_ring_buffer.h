#ifndef AOC_TREE
#define AOC_TREE
#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <stddef.h>

#define FOREACHCELL(_index, _buffer) \
    for (size_t _index = 0; _index < _buffer->_uArrDim; _index++)

typedef struct
{
    //   aoc_linked_list_t _items;
    void **_pvBuff;
    size_t _pxHead;
    size_t _pxTail;
    size_t _uAvailable;
    size_t _uCelLDim;
    size_t _uArrDim;
} ring_buffer_t;

typedef ring_buffer_t *aoc_ring_buffer_t;

aoc_ring_buffer_t aoc_ring_buffer(size_t _size, size_t _len);
void aoc_ring_buffer_free(aoc_ring_buffer_t _rbuf);

int aoc_ring_buffer_send_front(aoc_ring_buffer_t _pxRbuf, void *_item);
int aoc_ring_buffer_send_back(aoc_ring_buffer_t _pxRbuf, void *_item);

int aoc_ring_buffer_receive_front(aoc_ring_buffer_t _pxRbuf, void *_item);
int aoc_ring_buffer_receive_back(aoc_ring_buffer_t _pxRbuf, void *_item);

#endif