#include <errno.h>
#include "aoc_types.h"
#include "aoc_ranges.h"
#include "aoc_linked_list.h"

#define NAME_FMT "%16s"
#define ABSOLUTE_MAX_SYM_CNT (32)
#define ABSOLUTE_MAX_NAME_LEN (16)

typedef struct context_t* aoc_context_t;

/**
 * Text input parsers
 */

struct parser_t
{
    struct ll_node_t _node;
    char *_name;
    char *_parseRegx;
    size_t _parsed;
    int (*_func)(aoc_context_t _ctx, char *_str);
    void *_arg;
};
typedef struct parser_t * aoc_parser_t;


int aoc_parser_append(aoc_linked_list_handle_t _ll, const aoc_parser_t const _prs);
void aoc_parser_free(void *_data); 