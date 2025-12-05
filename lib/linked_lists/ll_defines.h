
#define DLL_DECLARE(_type) \
    struct dll_node _node;

#define DLL_NODE_CTOR(_type, _name)           \
    _type *_name(void)                        \
    {                                         \
        _type *_ret = NULL;                   \
        TRY_RAII_MALLOC(_ret, sizeof(_type)); \
        if (!_ret)                            \
            return NULL;                      \
        return _ret;                          \
    }

#define LL_FOREACH_P_EXT(_it, _head_h)                         \
    for (_it = _head_h->_first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH_P(_it, _head_h)                             \
    dll_node_h _it;                                            \
    for (_it = _head_h->_first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH(_it, _head)                              \
    dll_node_h _it;                                         \
    for (_it = _head._first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH_EXT(_it, _head)                          \
    for (_it = _head._first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH_LAST_EXT(_it, _head)                    \
    for (_it = _head._last; _it != NULL; _it = _it->_prev) \
        if (_it)

#define LL_FOREACH_LAST(_it, _blk) \
    dll_node_h _it;                \
    LL_FOREACH_LAST_EXT(_it, _blk)
