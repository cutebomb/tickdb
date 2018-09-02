#ifndef _TS_HASHTABLE
#define _TS_HASHTABLE

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "ts_list.h"

typedef unsigned (*ts_hash_function_t)(const char* key, unsigned int prime);

typedef struct ts_hash_table
{
	char *type;
	ts_hash_function_t hash;
	unsigned int prime;   // size of the array of pointer to list
	unsigned int vsize;   // size of the "value"
	unsigned int mentry;  // size of val_ar
	unsigned int nentry;
  
	ts_list_elem_t  **key_index_ar; // has (key, index) pair. Index points to val_ar, where *value is stored
	void **val_ar;
} ts_hash_table_t;

ts_hash_table_t *ts_hash_table_new(unsigned int size, int vsize);
ts_hash_table_t *ts_hash_table_with_new(unsigned int size, int vsize, char *type, ts_hash_function_t hash);

int ts_hash_table_insert(ts_hash_table_t * htbl, const char *key, void *value);
int ts_hash_table_delete(ts_hash_table_t * htbl, const char *key);

void *ts_hash_table_get(const ts_hash_table_t * htbl, const char *key);
int ts_hash_table_get_keys(const ts_hash_table_t *htbl, unsigned int n, char **key_ar);
int ts_hash_table_get_vals(ts_hash_table_t *htbl, unsigned int n, void **val_ar);
int ts_hash_table_apply(ts_hash_table_t *htbl, int(*action)(void *));
unsigned int ts_hash_table_write(const ts_hash_table_t *htbl, FILE *fp);
unsigned int ts_hash_table_read(ts_hash_table_t **htbl, FILE *fp, ts_hash_function_t hash);
int ts_hash_table_free(ts_hash_table_t * htbl);

#endif
