#ifndef AOC_LL
#define AOC_LL

#include <ll_types.h>
#include <ll_defines.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define LL_MAX_LEN_LUI (65535)
#define LL_MAX_LEN_STR "65535"

#define NODE_CAST(_p) ((dll_node_h)_p)
#define MAX_STR_DLL_LEN (1024)


void dll_head_init(dll_head_h head);
int dll_sort(dll_head_h head, dll_compare(*comp));
void dll_free_all(dll_head_h head, void (*_caller)(void *_data));

void dll_foreach(dll_head_h head, void *arg, void(func)(void *arga, void *argb));

int dll_node_append(dll_head_h head, dll_node_h _new);
int dll_find_node(dll_head_h head, dll_node_h _a);
int dll_node_insert(dll_head_h head, dll_node_h _a, dll_node_h _b);
int dll_node_sorted_insert(dll_head_h head, dll_node_h _new, dll_compare sort);
int dll_trim_nodes(dll_head_h head, void *arg, bool (*ifeq)(void *_a, void *_b), void(free)(void *arg));

dll_head_h dll_clone_trimmed(dll_head_h head, size_t nodesize, void *_prop, bool (*remiftrue)(void *arg, void *prop));
void dll_node_swap(dll_head_h head, dll_node_h _a, dll_node_h _b);
void dll_node_disconnect(dll_head_h head, dll_node_h _a);

size_t dll_size(dll_head_h head);
size_t dll_count_nodes_by_property(dll_head_h head, void *_prop, bool (*equal)(void *_a, void *_b));
dll_head_h dll_clone_sorted(dll_head_h head, dll_compare(*comp), size_t nodesize);

dll_node_h dll_find_node_by_property(dll_head_h head, void *_prop, bool (*equal)(void *_a, void *_b));


void string_dll_free(void *string);

string_dll_node_h string_dll(const char *const name);

dll_node_h string_dll_compare(dll_node_h _a, dll_node_h _b);

int string_dll_rename(string_dll_node_h node, const char *const name);


#endif