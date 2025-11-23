#include <ansi_cmd.h>
#include <aoc_types.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#define ANSI_LONGEST_CMD_IN_BYTES (8)

int aoc_inputs_ansi_to_dir(const char *const _str, AOC_2D_DIR *_dir);