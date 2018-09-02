#ifndef TS_STACK_H
#define TS_STACK_H

#include "ts_prgm.h"

typedef struct ts_stack { 
  int m;
  ts_stack_elem_t *stack_ar;
  int top;
} ts_stack_t;


ts_stack_t* ts_stack_new(void);
int ts_stack_free(ts_stack_t* stack);

int ts_stack_not_empty(ts_stack_t *);

int ts_stack_push(ts_stack_t *stack, ts_stack_elem_t *data);
int ts_stack_pop(ts_stack_t *stack);
int ts_stack_pop_to(ts_stack_t *stack, ts_stack_elem_t *v);
ts_stack_elem_t *ts_stack_top(ts_stack_t *stack);
int ts_stack_clear(ts_stack_t *stack);
ts_stack_elem_t *ts_stack_elem_copy(ts_stack_elem_t *o);

#endif
