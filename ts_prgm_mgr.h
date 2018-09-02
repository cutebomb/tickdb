#ifndef TS_PRGM_MGR_H
#define TS_PRGM_MGR_H

#include "ts_tree.h"
#include "ts_prgm.h"
#include "ts_hash_table.h"

typedef struct ts_prgm_mgr {
  ts_prgm_t *before;
  ts_prgm_t *after;
  ts_prgm_t *cur_formula;

  int mp;
  int np;
  ts_prgm_t **prgms_ar;

  ts_hash_table_t *pt;
  char *basedir;
} ts_prgm_mgr_t;

ts_prgm_mgr_t *ts_prgm_mgr_new(void);
int ts_prgm_mgr_free(ts_prgm_mgr_t *pm);
int ts_prgm_mgr_clear(ts_prgm_mgr_t *pm);
#ifdef DEBUG
int ts_prgm_mgr_print(ts_prgm_mgr_t *pm);
#endif

int ts_prgm_mgr_load_main(ts_prgm_mgr_t *pm, const char *path, int isstdin);
int ts_prgm_mgr_load_lib(ts_prgm_mgr_t *pm, const char *pname, short *res_pid);
#endif
