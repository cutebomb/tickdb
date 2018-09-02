#ifndef TS_TABLE_H
#define TS_TABLE_H

#include "ts_symbol_id_factory.h"
#include "ts_type.h"
#include "ts_var_mgr.h"

typedef struct ts_table_meta {
  short id; // table id
  char *tablename;
  char **colname_ar;
  short ncol;
  ts_type_t *type_ar;
  int *size_ar;
  short par_col;
  int sub_par_by;
  short grp_col;
} ts_table_meta_t;

ts_table_meta_t *ts_table_meta_new(const char *tablename, short ncol, const char **colname_ar, ts_type_t *type_ar, char *partition_by, char *group_by);
int ts_table_meta_free(ts_table_meta_t *meta);

typedef struct ts_table {
  ts_table_meta_t *meta;
  ts_symbol_id_factory_t *idf;

  // row data
  // int ttl_row;
  int mgrp;
  int ngrp;        // mem length, use m'version of var  means that dimension can dynamic be expended
  int *mrow_ar;    // x mgrp
  int *nrow_ar;    // x mgrp
  void *grp_v_ar;    // x mgrp
  void ***p_col_ar;// x ncol x mgrp x mrow

  short tcol;
  short nttlcol;
  short mttlcol;
  char **varname_ar;
  void ***var_col_ar;
  ts_type_t *var_type_ar;
  int *is_out_var_ar;
  int *var_size_ar;
  ts_hash_table_t *var_table;
    
  //partition data
  int mpar;
  int npar;
  int *cur_par_start_ar; // x mgrp partition edge in groups
  int *par_v_ar;

  int cur_par;
  int *cur_row_ar; // x mgrp
  int nlag;
  int dirty;

  //ts_time_t start;
  //ts_time_t end;
  ts_var_mgr_t *var_mgr;

  int cur_melem;
  int cur_nelem;
  int *cur_seq_ar;
} ts_table_t;

ts_table_t *ts_table_create(const char *tablename, short ncol, const char **colname_ar, ts_type_t *type_ar, char *partition_by, char *group_by);
int ts_table_append_row(ts_table_t *t, const void **data_ar);
//int ts_table_create_key(ts_table_t *t, int nkey, const char **key_ar);
int ts_table_write_meta(ts_table_t *t);
int ts_table_free(ts_table_t *t);
int ts_table_clear(ts_table_t *t);

ts_table_meta_t *ts_table_read_meta(const char *tablename);
int ts_table_write(ts_table_t *t);
ts_table_t *ts_table_read(const char *tablename);

int ts_table_append_from_csv(ts_table_t *t, const char *filename, int *filter_col_ar, const char *time_format);
int ts_table_partition_reset(ts_table_t *t);
int ts_table_partition_next(ts_table_t *t);

int ts_table_cur_var_print(ts_table_t *t, int gid, int offset);
int ts_table_copy_previous_var_all(ts_table_t *t, int gid);
int ts_table_copy_previous_var_part(ts_table_t *t, int gid, int nvar, int *varid_ar);
//int ts_table_set_start(ts_table_t *t, int idx, int value);
int ts_table_new_var(ts_table_t *t, const char *varname, ts_type_t type, int is_out, short *result);
unsigned ts_table_translate_gid(ts_table_t *from, ts_table_t *to, unsigned gid);

inline short ts_table_get_var_id(ts_table_t *t, const char *varname);
inline ts_type_t ts_table_get_var_type(ts_table_t *t, short varid);

inline int ts_table_set_vari(ts_table_t *t, short varid, int gid, int v);
inline int ts_table_set_varf(ts_table_t *t, short varid, int gid, float v);
inline int ts_table_set_varlf(ts_table_t *t, short varid, int gid, double v);

inline int ts_table_get_vari(ts_table_t *t, short varid, int gid, int *v);
inline int ts_table_get_varf(ts_table_t *t, short varid, int gid, float *v);
inline int ts_table_get_varlf(ts_table_t *t, short varid, int gid, double *v);
inline int ts_table_get_varsym(ts_table_t *t, short varid, int gid, char **v);

inline int ts_table_get_vari_ref(ts_table_t *t, short varid, int gid, int offset, int *v);
inline int ts_table_get_varf_ref(ts_table_t *t, short varid, int gid, int offset, float *v);
inline int ts_table_get_varlf_ref(ts_table_t *t, short varid, int gid, int offset, double *v);

//////////////////////////functions//////////////////////////////////////////////
typedef struct ts_sum_store {
} ts_sum_store_t;
inline int ts_table_sum(ts_table_t *t, short varid, int gid, int offset, ts_sum_store_t *st, ts_type_t vt, void *v);
typedef struct ts_sma_store {
  int lastidx;
  double lastsum;
  double lastold;
  double lastnew;
} ts_sma_store_t;
inline int ts_table_sma(ts_table_t *t, short varid, int gid, int length, ts_sma_store_t *st, ts_type_t vt, void *v);
typedef struct ts_ema_store {
  double a;
  double b;
  int lastidx;
  double lastema;
  double lastlastema;
} ts_ema_store_t;
inline int ts_table_ema(ts_table_t *t, short varid, int gid, int length, ts_ema_store_t *st, ts_type_t vt, void *v);
typedef struct ts_ama_store {
  int lastidx;
  double fctr;
  double lasttc;
  double lastoldchg;
  double lastnewchg;
  double lastama;
  double lastlastama;
} ts_ama_store_t;
inline int ts_table_ama(ts_table_t *t, short varid, int gid, int ERlen, int Flen, int Slen, ts_ama_store_t *st, ts_type_t vt, void *v);

inline int ts_table_hhv(ts_table_t *t, short varid, int gid, int length, ts_type_t vt, void *v);
inline int ts_table_llv(ts_table_t *t, short varid, int gid, int length, ts_type_t vt, void *v);

#endif
