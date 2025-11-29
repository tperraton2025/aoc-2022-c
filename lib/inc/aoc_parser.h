#include <errno.h>
#include "aoc_types.h"
#include "aoc_ranges.h"
#include "aoc_linked_list.h"

#define NAME_FMT "%16s"
#define ABSOLUTE_MAX_SYM_CNT (32)
#define ABSOLUTE_MAX_NAME_LEN (16)

typedef struct context *aoc_context_h;

/**
 * Text input parsers
 */

struct parser
{
    struct dll_node _node;
    char *_name;
    char *_parseRegx;
    char *_expMatches;
    size_t _parsed;
    int (*_func)(aoc_context_h _ctx, char *_str);
};
typedef struct parser *aoc_parser_h;

int aoc_parser_append(aoc_ll_head_h _ll, const aoc_parser_h const _prs);
void aoc_parser_free(void *_data);