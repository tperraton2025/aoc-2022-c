#ifndef AOC_STACK_H
#define AOC_STACK_H

#include <aoc_helpers.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct aoc_stack_t* aoc_stack_handle_t;

aoc_stack_handle_t aoc_stack_init(void (*_free)(void *args), size_t _size);
int aoc_stack_push(aoc_stack_handle_t _stkh, void *_new);
int aoc_stack_pop(aoc_stack_handle_t _stkh, void *_item);
void aoc_stack_free(void* _stkh);

#endif