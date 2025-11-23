#ifndef AOC_RANGES
#define AOC_RANGES

#include <stddef.h>
struct range_t
{
    size_t _max;
    size_t _min;
};

#define A_AROUND_B(_a, _b) (_a._max >= _b._max) && (_a._min <= _b._min)
#define N_IN_RANGE(_n, _a, _b) (((_n) >= (_a)) && ((_n) <= (_b)))

#define HIGHEST(_a, _b) ((_a) > (_b) ? (_a) : (_b))
#define LOWEST(_a, _b) ((_a) > (_b) ? (_b) : (_a))
#endif