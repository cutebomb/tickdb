#ifndef TS_VM_H
#define TS_VM_H

#include "ts_stack.h"
#include "ts_hash_table.h"
#include "ts_table.h"
#include "ts_type.h"
#include "ts_func_mgr.h"
#include "ts_var_mgr.h"
#include "ts_prgm_mgr.h"
#include "ts_bcktst_mgr.h"

typedef enum ts_vm_run_mode {
  VM_MODE_MAIN,
  VM_MODE_LIB,
  VM_MODE_SEQ,
  VM_MODE_MAX,
} ts_vm_run_mode_t;

typedef struct ts_vm {
  int l;
  int c;

  ts_stack_t *stack;

  ts_func_mgr_t *func_mgr;
    
  // time-series tables table_name->table_meta
  ts_hash_table_t *table_meta_table;
  
  // real table
  int mtable;
  int ntable;
  ts_table_t **table_ar;

  ts_table_t *cur_table;
  char *time_format;

  int cur_gid;  
  int run_gid;
  int display;

  int ngrp;
  int nfunc;
  void ***func_store_ar;

  ts_bcktst_mgr_t *btm;
  ts_prgm_mgr_t *prgm_mgr;
  int threads;
  
  double start;
  double end;
} ts_vm_t;

ts_vm_t *ts_vm_new(ts_prgm_mgr_t *pm);
int ts_vm_free(ts_vm_t *vm);
short ts_vm_get_table_id(ts_vm_t *vm, const char *tablename);
int ts_vm_run_once(ts_vm_t *vm, ts_prgm_t *prgm);
int ts_vm_run(ts_vm_t *vm, ts_prgm_t *prgm, ts_vm_run_mode_t mode);
int ts_vm_get_var(ts_vm_t *vm, short tid, short vid, ts_stack_elem_t *v);
int ts_vm_set_var(ts_vm_t *vm, short tid, short vid, ts_stack_elem_t *v);
int ts_vm_clear(ts_vm_t *vm);
int ts_vm_table_load(ts_vm_t *vm, char *tablename, ts_table_t **restbl, short *resid);

int ts_vm_get_var_valuelf(ts_vm_t *vm, ts_var_mgr_t *var_mgr, ts_stack_elem_t *a, double *v);
int ts_vm_get_var_valuef(ts_vm_t *vm, ts_var_mgr_t *var_mgr, ts_stack_elem_t *a, float *v);
int ts_vm_get_var_valuei(ts_vm_t *vm, ts_var_mgr_t *var_mgr, ts_stack_elem_t *a, int *v);

#endif
