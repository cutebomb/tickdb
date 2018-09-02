#ifndef TS_FUNC_MGR_H
#define TS_FUNC_MGR_H

#include "ts_tree.h"
#include "ts_hash_table.h"
#include "ts_func.h"

typedef struct ts_func_meta {
  int id;
  char *func_name;
  int nsvar;
  int *sfunc_svar_idx_ar;
  ts_func_t func;
} ts_func_meta_t;

int ts_func_meta_free(ts_func_meta_t *meta);

typedef struct ts_func_mgr{
  int nfunc;
  int mfunc;
  ts_func_meta_t **func_meta_ar;
  ts_hash_table_t *func_meta_table;
  ts_func_t *func_ar;
} ts_func_mgr_t;

ts_func_mgr_t *ts_func_mgr_new(void);
int ts_func_mgr_free(ts_func_mgr_t *mgr);
int ts_func_mgr_register_func(ts_func_mgr_t *mgr, const char *funcname, int nsvar, int *svar_ar, ts_func_t func);
int ts_func_mgr_get_funcid(ts_func_mgr_t *mgr, const char *func, short *rv);
int ts_func_mgr_is_sfunc(ts_func_mgr_t *mgr, int funcid);
int ts_func_mgr_get_sfunc_svar_ar(ts_func_mgr_t *mgr, int funcid, int *nsvar, int **psvaridx_ar);
int ts_func_mgr_call(ts_func_mgr_t *mgr, void *vm, int funcid, int argc, ts_stack_elem_t *argv, void *rv);

#endif
