#include "aoc_numeric.h"

base10_h base_10_conversion(size_t luint)
{
    base10_h _base10 = malloc(sizeof(base10_t));
    if (!_base10)
        return NULL;
    memset(_base10, 0, sizeof(base10_t));
    _base10->_val = luint;
    for (size_t _ii = 0; _ii < ARRAY_DIM(_base10->_digits) && luint; _ii++)
    {
        _base10->_lastdigit++;
        _base10->_digits[_ii] = (char)(luint % 10);
        luint = luint / 10;
    }
    size_t rconv = base_10_reverse_conversion(_base10);
    if (_base10->_val != rconv)
    {
        aoc_err("initial conv failed with %ld != %ld", _base10->_val, rconv);
    }
    return _base10;
}

size_t base_10_reverse_conversion(base10_h base10)
{
    size_t _ret = 0;
    size_t _pow = 1;
    for (size_t _ii = 0; _ii < base10->_lastdigit; _ii++)
    {
        _ret += base10->_digits[_ii] * (_pow);
        _pow *= 10;
    }
    base10->_val = _ret;
    return _ret;
}

void print_base10(base10_h base10)
{
    base10_t _dummy = {0};

    for (size_t _ii = 0; _ii <= base10->_lastdigit; _ii++)
    {
        _dummy._digits[_ii] = base10->_digits[_ii] + '0';
    }
    aoc_info("%lu = %s", base10->_val, &_dummy._digits[0]);
}

char *string_reverse(char *_str)
{
    size_t _len = strlen(_str);
    for (size_t _ii = 0; _ii < _len >> 1; _ii++)
    {
        _str[_ii] = _str[_len - 1 - _ii] ^ _str[_ii];
        _str[_len - 1 - _ii] = _str[_ii] ^ _str[_len - 1 - _ii];
        _str[_ii] = _str[_len - 1 - _ii] ^ _str[_ii];
    }
    return _str;
}

char *string_base10(base10_h base10)
{
    char *_str = malloc(base10->_lastdigit + 1);
    size_t _ii = 0;
    for (; _ii < base10->_lastdigit; _ii++)
    {
        _str[_ii] = base10->_digits[_ii] + '0';
    }
    _str[_ii + 1] = '\0';
    return _str;
}

int base_10_increment(base10_h base10)
{
    base10->_val++;
    for (size_t _ii = 0; _ii < ARRAY_DIM(base10->_digits); _ii++)
    {
        if (++base10->_digits[_ii] >= 10)
        {
            if (_ii >= base10->_lastdigit - 1)
                base10->_lastdigit++;

            base10->_digits[_ii] %= 10;
            if (_ii == ARRAY_DIM(base10->_digits) - 1)
                return EOVERFLOW;
        }
        else
            break;
    }

    size_t rconv = base_10_reverse_conversion(base10);
    if (base10->_val != rconv)
    {
        aoc_err("increment failed with %ld != %ld", base10->_val, rconv);
    }
    return 0;
}