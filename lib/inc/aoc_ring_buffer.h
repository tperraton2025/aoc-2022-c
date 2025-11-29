#ifndef AOC_TREE
#define AOC_TREE
#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <stddef.h>

#define FOREACHCELL(_index, _buffer) \
    for (size_t _index = 0; _index < _buffer->_u_arr_dim; _index++)

typedef struct ring_buffer *aoc_ring_buffer_h;

aoc_ring_buffer_h aoc_ring_buffer(size_t _size, size_t _len);
void aoc_ring_buffer_free(aoc_ring_buffer_h _rbuf);
size_t aoc_ring_buffer_available(aoc_ring_buffer_h _rbuf);
size_t aoc_ring_buffer_item_size(aoc_ring_buffer_h _rbuf);

int aoc_ring_buffer_send_front(aoc_ring_buffer_h _pxRbuf, void *_item);
int aoc_ring_buffer_send_back(aoc_ring_buffer_h _pxRbuf, void *_item);

int aoc_ring_buffer_receive_front(aoc_ring_buffer_h _pxRbuf, void *_item);
int aoc_ring_buffer_receive_back(aoc_ring_buffer_h _pxRbuf, void *_item);

#endif