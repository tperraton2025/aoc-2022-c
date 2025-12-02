#ifndef parser_H
#define parser_H
#include <errno.h>
#include "aoc_types.h"
#include "aoc_ranges.h"
#include "aoc_linked_list.h"

#define NAME_FMT "%16s"
#define ABSOLUTE_MAX_PART_CNT (32)
#define ABSOLUTE_MAX_NAME_LEN (16)

typedef struct context *aoc_context_h;

/**
 * Text input parsers
 */

struct parser
{
    struct dll_node _node;
    char *_name;
    void *arg;
    size_t _parsed;
    int (*_func)(void *arg, char *_str);
};
typedef struct parser *parser_h;

int parser_append(dll_head_h _ll, parser_h parser, void *arg);
int parse_all(dll_head_h _ll, char *str);
void parser_free(void *_data);
#endif