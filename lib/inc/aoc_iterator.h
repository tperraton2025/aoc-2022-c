#ifndef AOC_ITERATOR_H
#define AOC_ITERATOR_H

#define FOREACH_P_NONNULL(_it, _arr) for (size_t _it = 0; (NULL != _arr[_it]) && _it < (sizeof(_arr) / sizeof(_arr[0])); _it++)

#endif