#ifndef _TS_LIST
#define _TS_LIST

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef int(*ts_list_compare_t)(const void* left, const void* right);

typedef struct ts_list_elem
{
	int size;
	void *value;
	struct ts_list_elem  *next;
} ts_list_elem_t;

ts_list_elem_t *ts_list_elem_new(void *value, const size_t size);
int ts_list_elem_free(ts_list_elem_t *elem);
unsigned ts_list_count(const ts_list_elem_t* elem);
int ts_list_append_elem(ts_list_elem_t **head, ts_list_elem_t *elem);
int ts_list_append(ts_list_elem_t **head, void *value, size_t sz);

int ts_list_prepend_elem(ts_list_elem_t **head, ts_list_elem_t *elem);
int ts_list_prepend(ts_list_elem_t **head, void *value, size_t sz);
int ts_list_insert_elem(ts_list_elem_t **after, ts_list_elem_t *elem);
int ts_list_insert(ts_list_elem_t **after, void *value, size_t sz);
int ts_list_remove(ts_list_elem_t **head, const void *target, ts_list_compare_t cmpfunc);
void *ts_list_pop(ts_list_elem_t **head); // remove the first element, return value
ts_list_elem_t *ts_list_search_elem(ts_list_elem_t **head, const void *target, ts_list_compare_t cmpfunc);

int ts_list_map(ts_list_elem_t **head, int (*func)(void *));
#define ts_list_map0(list, func) ts_list_map((list), (int(*)(void*))(func))

int ts_list_free(ts_list_elem_t **head);   

#endif

