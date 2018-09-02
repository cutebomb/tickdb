#include "ts_stack.h"
#include "ts_misc.h"
#include <stdlib.h>
#include <assert.h>

ts_stack_t* ts_stack_new(void)
{
  ts_stack_t* stack = 0;
  
  TS_TRY(stack = (ts_stack_t*)calloc(1, sizeof(ts_stack_t)));
  stack->m = 1000;
  TS_TRY(stack->stack_ar = (ts_stack_elem_t*)calloc(stack->m, sizeof(ts_stack_elem_t)));
  
  return(stack);
 fail:
  if(stack) {
    if(! ts_stack_free(stack)) {
      (void) ts_warn(stderr, "\n");
      return 0;
    }
  }
  return 0;
}

int ts_stack_free(ts_stack_t* stack)
{
  TS_TRY(stack);

  ts_free(stack->stack_ar);
  free(stack);

  return 1;
 fail:
  return 0;
}

int ts_stack_not_empty(ts_stack_t * stack)
{
  assert(stack);

  return stack->top != 0;
}

int ts_stack_push(ts_stack_t *stack, ts_stack_elem_t* data)
{
  TS_TRY(stack && data);

  if(stack->top == stack->m) {
    ts_realloc(stack->stack_ar, stack->m, 1000, ts_stack_elem_t);
    stack->m += 1000;
  }
  memcpy(&stack->stack_ar[stack->top], data, sizeof(ts_stack_elem_t));
  stack->top++;
  
  return 1;
 fail:
  return 0;
}

int ts_stack_pop(ts_stack_t *stack)
{
  TS_TRY(stack && stack->top > 0);
  --stack->top;
  return 1;
 fail:
  return 0;
}

int ts_stack_pop_to(ts_stack_t *stack, ts_stack_elem_t *v)
{
  TS_TRY(stack && stack->top > 0);
  --stack->top;
  memcpy(v, &stack->stack_ar[stack->top], sizeof(ts_stack_elem_t));
  return 1;
 fail:
  return 0;
}

ts_stack_elem_t *ts_stack_top(ts_stack_t *stack)
{
  TS_TRY(stack);
  return &stack->stack_ar[stack->top-1];
 fail:
  return 0;
}

int ts_stack_clear(ts_stack_t *stack)
{
  TS_TRY(stack);
  stack->top = 0;
  return 1;
 fail:
  return 0;
}
    
