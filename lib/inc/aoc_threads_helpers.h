#ifndef AOC_THREADS_HELPERS
#define AOC_THREADS_HELPERS

#define ASSIGN_AND_RETURN_IF_NOT_0(_var, _expr, _error) \
    _var = _expr;                               \
    if (_var)                                   \
        goto _error;

#define ALLOCATE_AND_RETURN_IF_NULL(_var, _expr, _ret, _err, _goto) \
    _var = _expr;                                            \
    if (!_var)                                               \
    {                                                        \
        _ret = _err;                                         \
        goto _goto;                                          \
    }

#endif