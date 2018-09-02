#include "ts_var_mgr.h"
#include "ts_misc.h"

static inline int _ts_var_mgr_reset_ngrp(ts_var_mgr_t *v, int ngrp)
{
  TS_TRY(ngrp > v->ngrp);
  int i;
  ts_realloc(v->st_ar, v->ngrp, ngrp - v->ngrp, double*);
  for(i=v->ngrp;i<ngrp;i++) {
    TS_TRY(v->st_ar[i] = calloc(v->mv, sizeof(double)));
  }
  v->ngrp = ngrp;
  return 1;
 fail:
  return 0;
}

ts_var_mgr_t *ts_var_mgr_new(int ngrp)
{
  ts_var_mgr_t *v = calloc(1, sizeof(ts_var_mgr_t));
  TS_TRY(v);
  v->mv = 100;
  v->nv = 0;

  TS_TRY(v->svarname = calloc(v->mv, sizeof(char*)));
  TS_TRY(v->vt = ts_hash_table_new(500, sizeof(short)));
  TS_TRY(_ts_var_mgr_reset_ngrp(v, ngrp));

  
  return v;
 fail:
  return 0;
}

int ts_var_mgr_free(ts_var_mgr_t* v)
{
  TS_TRY(v);
  if(v->vt) {
    ts_hash_table_free(v->vt);
    v->vt = 0;
  }
  int i;
  for(i=0;i<v->ngrp;i++) {
    ts_free(v->st_ar[i]);
  }
  for(i=0;i<v->nv;i++) {
    ts_free(v->svarname[i]);
  }
  ts_free(v->svarname);
  ts_free(v->st_ar);
  free(v);
  return 1;
 fail:
  return 0;
}

int ts_var_mgr_new_var(ts_var_mgr_t *v, const char *varname, short *vid)
{
  TS_TRY(v && varname && vid);
  int i;
  if(v->nv == v->mv) {
    for(i=0;i<v->ngrp;i++) {
      ts_realloc(v->st_ar[i], v->mv, 100, double*);
    }
    v->mv += 100;
  }
  *vid = v->nv;
  v->svarname[v->nv] = strdup(varname);

  TS_TRY(ts_hash_table_insert(v->vt, varname, vid));

  v->nv++;
  return 1;
 fail:
  return 0;
}

short ts_var_mgr_get_var_id(ts_var_mgr_t *v, const char *varname)
{
  TS_TRY(v && varname);
  short *ptr;
  ptr = ts_hash_table_get(v->vt, varname);
  if(ptr) {
    TS_TRY(*ptr >= 0 && *ptr < v->nv);
    return *ptr;
  } else {
    return -1;
  }
 fail:
  return -1;
}

int ts_var_mgr_get_var(ts_var_mgr_t *v, short vid, int gid, double *vl)
{
  TS_TRY(v && gid >= 0 && vid >= 0 && vid < v->nv);
  if(gid >= v->ngrp) {
    TS_TRY(_ts_var_mgr_reset_ngrp(v, gid+100));
  }
  *vl = v->st_ar[gid][vid];
  return 1;
 fail:
  return 0;
}

int ts_var_mgr_set_var(ts_var_mgr_t *v, short vid, int gid, double vl)
{
  TS_TRY(v && gid >= 0 && vid >= 0 && vid < v->nv);
  if(gid >= v->ngrp) {
    TS_TRY(_ts_var_mgr_reset_ngrp(v, gid+100));
  }
  v->st_ar[gid][vid] = vl;
  return 1;
 fail:
  return 0;
}
