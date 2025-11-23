#include <aoc_input.h>

int aoc_inputs_ansi_to_dir(const char *const _str, AOC_2D_DIR *_dir)
{
    if (!_str)
        return EINVAL;
    if ('\e' != _str[0])
        return EINVAL;

    size_t _len = strnlen(_str, ANSI_LONGEST_CMD_IN_BYTES);

    if (('[' == _str[1]) && isdigit(_str[2]))
        switch (_str[3])
        {
        case 'A':
            *_dir = AOC_DIR_UP;
            return 0;
        case 'B':
            *_dir = AOC_DIR_DOWN;
            return 0;
        case 'C':
            *_dir = AOC_DIR_RIGHT;
            return 0;
        case 'D':
            *_dir = AOC_DIR_LEFT;
            return 0;
        default:
            return EINVAL;
        }
}
 