#include "ts_func_mgr.h"
#include "ts_vm.h"

extern ts_vm_t *vm;

ts_func_mgr_t *ts_func_mgr_new(void)
{
  ts_func_mgr_t *mgr = calloc(1, sizeof(ts_func_mgr_t));
  TS_TRY(mgr);
  mgr->nfunc = 0;
  mgr->mfunc = 100;
  TS_TRY(mgr->func_meta_table = ts_hash_table_new(100, 0));
  TS_TRY(mgr->func_meta_ar = calloc(mgr->mfunc, sizeof(ts_func_meta_t*)));
  TS_TRY(mgr->func_ar = calloc(mgr->mfunc, sizeof(ts_func_t)));
  return mgr;
 fail:
  return 0;
}

int ts_func_mgr_free(ts_func_mgr_t *mgr)
{
  TS_TRY(mgr);
  int i=0;
  ts_free(mgr->func_ar);
  if(mgr->func_meta_table) {
    ts_hash_table_free(mgr->func_meta_table);
  }
  for(i=0;i<mgr->nfunc;i++) {
    ts_func_meta_free(mgr->func_meta_ar[i]);
  }
  return 1;
 fail:
  return 0;
}

int ts_func_meta_free(ts_func_meta_t *meta)
{
  TS_TRY(meta);
  
  ts_free(meta->func_name);
  ts_free(meta->sfunc_svar_idx_ar);
  free(meta);
  
  return 1;
 fail:
  return 0;
}

int ts_func_mgr_register_func(ts_func_mgr_t *mgr, const char *funcname, int nsvar, int *svar_ar, ts_func_t func)
{
  TS_TRY(mgr && func && nsvar>=0);

  ts_func_meta_t *meta = calloc(1, sizeof(ts_func_meta_t));
  meta->id = mgr->nfunc;

  if(mgr->nfunc == mgr->mfunc) {
    ts_realloc(mgr->func_meta_ar, mgr->mfunc, 100, ts_func_meta_t);
    ts_realloc(mgr->func_ar, mgr->mfunc, 100, ts_func_t);
    mgr->mfunc += 100;
  }

  TS_TRY(meta->func_name = strdup(funcname));
  meta->func = func;
  if(nsvar > 0) {
    meta->nsvar = nsvar;
    TS_TRY(meta->sfunc_svar_idx_ar = memdup(svar_ar, nsvar*sizeof(int)));
  }
  TS_TRY(ts_hash_table_insert(mgr->func_meta_table, funcname, meta));
  mgr->func_meta_ar[mgr->nfunc] = meta;
  mgr->func_ar[mgr->nfunc] = func;
  mgr->nfunc++;
  
  return 1;
 fail:
  return 0;
}

int ts_func_mgr_get_funcid(ts_func_mgr_t *mgr, const char *func, short *rv)
{
  TS_TRY(mgr && func && rv);
  ts_func_meta_t *p;
  p = ts_hash_table_get(mgr->func_meta_table, func);
  if(p) {
    *rv = p->id;
  } else {
    (void)ts_warn(stderr, "runtime error @[%d|%d]:function %s not found.\n", vm->l, vm->c, func);
    goto fail;
  }
  return 1;
 fail:
  return 0;
}

int ts_func_mgr_is_sfunc(ts_func_mgr_t *mgr, int funcid)
{
  TS_TRY(mgr && funcid < mgr->nfunc);
  return mgr->func_meta_ar[funcid]->nsvar != 0;
 fail:
  return 0;
}

int ts_func_mgr_get_sfunc_svar_ar(ts_func_mgr_t *mgr, int funcid, int *nsvar, int **psvaridx_ar)
{
  TS_TRY(mgr && funcid < mgr->nfunc && nsvar && psvaridx_ar);
  *nsvar = mgr->func_meta_ar[funcid]->nsvar;
  *psvaridx_ar = mgr->func_meta_ar[funcid]->sfunc_svar_idx_ar;
  return 1;
 fail:
  return 0;
}

int ts_func_mgr_call(ts_func_mgr_t *mgr, void *vm, int funcid, int argc, ts_stack_elem_t *argv, void *rv)
{
  TS_TRY(mgr && vm && funcid >= 0 && funcid < mgr->nfunc && argv >= 0 && rv);
  TS_TRY(mgr->func_ar[funcid](vm, argc, argv, rv));
  return 1;
 fail:
  return 0;
}

