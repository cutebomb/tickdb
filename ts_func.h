#ifndef TS_FUNC_H
#define TS_FUNC_H

#include "ts_stack.h"

typedef int (*ts_func_t)(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);

int ts_func_abs(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_ref(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_max(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_min(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_sum(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_sma(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_ema(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_ama(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_hhv(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_llv(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);

int ts_func_table_create(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_table_from_csv(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_table_write(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_table_load(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_table_append(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_set_run_gid(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_set_display(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_load_module(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_set_time_format(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_get_other(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_set_threads(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_set_runrange(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_print(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
#endif
