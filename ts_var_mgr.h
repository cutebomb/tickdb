#ifndef TS_VAR_MGR_H
#define TS_VAR_MGR_H

#include "ts_hash_table.h"

typedef struct ts_var_mgr {
  int ngrp;
  int mv;
  int nv;
  double **st_ar;
  char **svarname;
  ts_hash_table_t *vt;
} ts_var_mgr_t;

ts_var_mgr_t *ts_var_mgr_new(int ngrp);
int ts_var_mgr_free(ts_var_mgr_t* v);

int ts_var_mgr_new_var(ts_var_mgr_t *v, const char *varname, short *vid);
short ts_var_mgr_get_var_id(ts_var_mgr_t *v, const char *varname);
int ts_var_mgr_set_var(ts_var_mgr_t *v, short vid, int gid, double vl);
int ts_var_mgr_get_var(ts_var_mgr_t *v, short vid, int gid, double *vl);

#endif
