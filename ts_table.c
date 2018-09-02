#include "ts_table.h"
#include "ts_csv_reader.h"
#include "ts_misc.h"
#include "ts_type.h"
#include "ts_vm.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <float.h>

extern ts_vm_t *vm;
static int ts_table_write_cur_partition(ts_table_t *t);
static int ts_table_get_time_par_v(ts_table_meta_t *meta, ts_time_t t);
static int ts_table_copy_meta_to_var(ts_table_t *t);
static ts_table_t *_ts_table_new(ts_table_meta_t *meta);
static int _ts_table_partition_read(ts_table_t *t, int npar);

ts_table_meta_t *ts_table_meta_new(const char *tablename, short ncol, const char **colname_ar, ts_type_t *type_ar, char *partition_by, char *group_by)
{
  TS_TRY(tablename && colname_ar && type_ar);
  ts_table_meta_t *meta = calloc(1, sizeof(ts_table_meta_t));
  int i;
  char *dot = 0, *e = 0;
  TS_TRY(meta);
  
  meta->par_col = -1;
  meta->grp_col = -1;
  meta->ncol = ncol;

  if(partition_by) {
    dot = strchr(partition_by, '.');
    if(dot) {
      *dot = '\0';
      e = dot+1;
      if(0 == strcmp(e, "day"))
	meta->sub_par_by = 1;
      else if(0 == strcmp(e, "month"))
	meta->sub_par_by = 2;
    }
  }

  TS_TRY(meta->tablename = strdup(tablename));
  TS_TRY(meta->colname_ar = calloc(meta->ncol, sizeof(char*)));
  TS_TRY(meta->type_ar = malloc(meta->ncol * sizeof(ts_type_t)));
  memcpy(meta->type_ar, type_ar, meta->ncol*sizeof(ts_type_t));
  TS_TRY(meta->size_ar = malloc(meta->ncol * sizeof(int)));

  for(i=0;i<ncol;i++) {
    TS_TRY(meta->colname_ar[i] = strdup(colname_ar[i]));
    if(type_ar[i] == TSTYPE_INT) {
      meta->size_ar[i] = sizeof(int);
    } else if(type_ar[i] == TSTYPE_FLOAT) {
      meta->size_ar[i] = sizeof(float);
    } else if(type_ar[i] == TSTYPE_DOUBLE) {
      meta->size_ar[i] = sizeof(double);
    } else if(type_ar[i] == TSTYPE_TIME) {
      meta->size_ar[i] = sizeof(ts_time_t);
    }
    if(partition_by) {
      if(0 == strcmp(colname_ar[i], partition_by)) {
	TS_TRY(type_ar[i] == TSTYPE_TIME); // only TSTYPE_TIME is allow split
	meta->par_col = i;
      }
    }
    if(group_by) {
      if(0 == strcmp(colname_ar[i], group_by)) {
	meta->grp_col = i;
	if(type_ar[i] != TSTYPE_SYM){
	  (void)ts_warn(stderr, "only sym can be the group key.\n");
	  goto fail;
	}
      }
    }
  }

  return meta;
 fail:
  return 0;
}

int ts_table_meta_free(ts_table_meta_t *meta)
{
  TS_TRY(meta);
  ts_free(meta->tablename);
  int i=0;
  for(i=0;i<meta->ncol;i++) {
    ts_free(meta->colname_ar[i]);
  }
  ts_free(meta->colname_ar);
  ts_free(meta->type_ar);
  ts_free(meta->size_ar);
  ts_free(meta);

  return 1;
 fail:
  return 0;
}

static ts_table_t *_ts_table_new(ts_table_meta_t *meta)
{
  TS_TRY(meta);
  int i=0;
  ts_table_t *t = calloc(1, sizeof(ts_table_t));
  t->meta = meta;
  TS_TRY(ts_table_copy_meta_to_var(t));

  t->nlag = 100;
  t->mgrp = 100;
  t->ngrp = 0;
  TS_TRY(t->mrow_ar = malloc(t->mgrp*sizeof(int)));
  for(i=0;i<t->mgrp;i++)
    t->mrow_ar[i] = 1000;
  TS_TRY(t->nrow_ar = calloc(t->mgrp, sizeof(int)));
  TS_TRY(t->grp_v_ar = calloc(t->mgrp, sizeof(void*)));
  TS_TRY(t->cur_row_ar = calloc(t->mgrp, sizeof(int)));

  if(t->meta->par_col != -1) {
    TS_TRY(t->cur_par_start_ar = calloc(t->mgrp, sizeof(int)));
    t->mpar = 100;
    t->npar = 0;
    TS_TRY(t->par_v_ar = calloc(t->mpar, sizeof(int)));
  }

  TS_TRY(t->p_col_ar = calloc(t->meta->ncol, sizeof(void**)));
  for(i=0;i<t->meta->ncol;i++) {
    if(meta->type_ar[i] == TSTYPE_TIME) {
      t->tcol = i;
    }
    if(i != meta->grp_col) {
      TS_TRY(t->p_col_ar[i] = calloc(t->mgrp, sizeof(void*)));
    } else {
      t->p_col_ar[i] = 0;
    }
  }
  TS_TRY(t->idf = ts_symbol_id_factory_new(1000 ,0 , TS_SYMBOL_ID_FACTORY_MODE_NONE));
  TS_TRY(t->var_mgr = ts_var_mgr_new(100));

  return t;
 fail:
  return 0;
}

int ts_table_free(ts_table_t *t)
{
  TS_TRY(t);
  int i,j;
  if(t->dirty) {
    ts_table_write(t);
  }

  ts_free(t->cur_row_ar);
  ts_free(t->par_v_ar);
  if(t->var_table) {
    ts_hash_table_free(t->var_table);
  }
  ts_free(t->var_size_ar);
  ts_free(t->is_out_var_ar);
  ts_free(t->var_type_ar);
  for(i=t->meta->ncol;i<t->nttlcol;i++) {
    for(j=0;j<t->ngrp;j++) {
      ts_free(t->var_col_ar[i][j]);
    }
    ts_free(t->var_col_ar[i]);
    ts_free(t->varname_ar);
  }
  for(i=0;i<t->meta->ncol;i++) {
    if(i != t->meta->grp_col) {
      for(j=0;j<t->ngrp;j++) {
	ts_free(t->p_col_ar[i][j]);
      }
      ts_free(t->p_col_ar[i]);
    }
  }
  ts_free(t->grp_v_ar);
  ts_free(t->nrow_ar);
  ts_free(t->mrow_ar);
  if(t->idf) {
    ts_symbol_id_factory_free(t->idf);
  }
  if(t->meta) {
    ts_table_meta_free(t->meta);
  }
  
  return 1;
 fail:
  return 0;
}

int ts_table_clear(ts_table_t *t)
{
  TS_TRY(t);
  int i,j,nc = t->meta->ncol, nv = t->nttlcol;
  for(i=nc;i<nv;i++) {
    TS_TRY(ts_hash_table_delete(t->var_table, t->varname_ar[i]));
    ts_free(t->varname_ar[i]);
    for(j=0;j<t->ngrp;j++) {
      ts_free(t->var_col_ar[i][j]);
    }
    ts_free(t->var_col_ar[i]);
  }
  memset(t->var_type_ar+nc, 0, nv-nc);
  memset(t->is_out_var_ar+nc, 0, nv-nc);
  memset(t->var_size_ar+nc, 0, nv-nc);
  t->nttlcol = t->meta->ncol;
  
  return 1;
 fail:
  return 0;
}

ts_table_t *ts_table_create(const char *tablename, short ncol, const char **colname_ar, ts_type_t *type_ar, char *partition_by, char *group_by)
{
  TS_TRY(tablename && colname_ar && type_ar);
  ts_table_t *t = 0;
  ts_table_meta_t *meta = 0;

  TS_TRY(meta = ts_table_meta_new(tablename, ncol, colname_ar, type_ar, partition_by, group_by));
  TS_TRY(t = _ts_table_new(meta));

  return t;
 fail:
  return 0;
}

int ts_table_append_from_csv(ts_table_t *t, const char* filename, int *filter_col_ar, const char *time_format)
{
  TS_TRY(t && filename);
    
  ts_csv_reader_t *rd = ts_csv_reader_new(filename);
  if(!rd) {
    (void)ts_warn(stderr, "runtime error @[%d|%d]: can not open csv file[%s].\n", vm->l, vm->c, filename);
    goto fail;
  }
  int i;
  int ncol = t->meta->ncol;
  void **data_ar = alloca(ncol*sizeof(void*));
  memset(data_ar, 0, ncol*sizeof(void*));
  while(-1 != ts_csv_reader_read_next(rd)) {
    for(i=0;i<ncol;i++) {
      switch(t->meta->type_ar[i]) {
      case TSTYPE_INT:
	if(!((int**)data_ar)[i]) {
	  ((int**)data_ar)[i] = (int*)alloca(sizeof(int));
	}
	if(filter_col_ar) {
	  TS_TRY(filter_col_ar[i] >= 0);
	  *((int**)data_ar)[i] = atoi(rd->elem_ar[filter_col_ar[i]]);
	} else {
	  *((int**)data_ar)[i] = atoi(rd->elem_ar[i]);
	}
	break;
      case TSTYPE_FLOAT:
	if(!((float**)data_ar)[i]) {
	  ((float**)data_ar)[i] = (float*)alloca(sizeof(float));
	}
	if(filter_col_ar) {
	  TS_TRY(filter_col_ar[i] >= 0);
	  *((float**)data_ar)[i] = atof(rd->elem_ar[filter_col_ar[i]]);
	} else {
	  *((float**)data_ar)[i] = atof(rd->elem_ar[i]);
	}
	break;
      case TSTYPE_DOUBLE:
	if(!((double**)data_ar)[i]) {
	  ((double**)data_ar)[i] = (double*)alloca(sizeof(double));
	}
	if(filter_col_ar) {
	  TS_TRY(filter_col_ar[i] >= 0);
	  *((double**)data_ar)[i] = atof(rd->elem_ar[filter_col_ar[i]]);
	} else {
	  *((double**)data_ar)[i] = atof(rd->elem_ar[i]);
	}
	break;
      case TSTYPE_SYM:
	if(!((char**)data_ar)[i]) {
	  ((char***)data_ar)[i] = alloca(sizeof(char*));
	}
	if(filter_col_ar) {
	  TS_TRY(filter_col_ar[i] >= 0);
	  *((char***)data_ar)[i] = rd->elem_ar[filter_col_ar[i]];
	} else {
	  *((char***)data_ar)[i] = rd->elem_ar[i];
	}
	break;
      case TSTYPE_TIME:
	if(!((ts_time_t**)data_ar)[i]) {
	  ((ts_time_t**)data_ar)[i] = alloca(sizeof(ts_time_t));
	}
	if(filter_col_ar) {
	  TS_TRY(filter_col_ar[i] >= 0);
	  *((ts_time_t**)data_ar)[i] = my_strptime(rd->elem_ar[filter_col_ar[i]], time_format);
	} else {
	  *((ts_time_t**)data_ar)[i] = my_strptime(rd->elem_ar[i], time_format);
	}
	break;
      default:
	(void)ts_warn(stderr, "\n");
	goto fail;
      }
    }
    TS_TRY(ts_table_append_row(t, (const void**)data_ar));
  }

  return 1;
 fail:
  return 0;
}

int ts_table_write_meta(ts_table_t *t)
{
  TS_TRY(t && t->meta);
  int i=0, rv=-1;
  char buf[1000],c = '\0';
  FILE *fp;
  struct stat st;
 
  snprintf(buf, 1000, "%s", t->meta->tablename);
  rv = mkdir(buf, S_IRWXU);
  TS_TRY(rv == 0 || errno == EEXIST);

  snprintf(buf, 1000, "%s/.m", t->meta->tablename);
  rv = stat(buf, &st);

  if(rv != 0) { // not meta exist file or error, try to new a meta file
    snprintf(buf, 1000, "%s/.m", t->meta->tablename);
    TS_TRY(fp = fopen(buf, "wb"));
    fwrite(&t->meta->ncol, sizeof(int), 1, fp);
    fwrite(&t->meta->par_col, sizeof(int), 1, fp);
    fwrite(&t->meta->grp_col, sizeof(int), 1, fp);
    fwrite(&t->meta->sub_par_by, sizeof(int), 1, fp);
    for(i=0;i<t->meta->ncol;i++) {
      fprintf(fp, "%s", t->meta->colname_ar[i]);
      fwrite(&c, sizeof(char), 1, fp);
      fprintf(fp, "%d", t->meta->type_ar[i]);
      fwrite(&c, sizeof(char), 1, fp);
    }
    fclose(fp);
  }

  return 1;
 fail:
  return 0;
}

int ts_table_append_row(ts_table_t *t, const void **data_ar)
{
  TS_TRY(t && data_ar);
  int i=0, gid, rowidx, par_v;
  
  if(!t->dirty) {
    t->dirty = 1;
  }

  if(t->meta->par_col != -1) {
    par_v = ts_table_get_time_par_v(t->meta, *((ts_time_t**)data_ar)[t->meta->par_col]);

    if(t->npar == 0) {
      t->cur_par = t->npar;
      t->npar++;
      t->par_v_ar[t->cur_par] = par_v;
    } else if(par_v != t->par_v_ar[t->cur_par]) {
      TS_TRY(ts_table_write_cur_partition(t));
      if(t->npar == t->mpar) {
	TS_TRY(t->par_v_ar = realloc(t->par_v_ar, (t->mpar+100)*sizeof(int)));
	memset(&t->par_v_ar[t->mpar], 0, 100*sizeof(int));
	t->mpar += 100;
      }
      t->cur_par = t->npar;
      t->npar++;
      t->par_v_ar[t->cur_par] = par_v;
    }
    /*
      if(t->npar == 0 || par_v != t->par_v_ar[t->cur_par]) {
      if(t->npar != 0) { // save old partition before add new partition
      TS_TRY(ts_table_write_cur_partition(t)); // write cur par and swap out cur partition except for last nlag data, modify cur_par_start_ar
      }
      t->cur_par = t->npar;
      if(t->npar == t->mpar) {
      TS_TRY(t->par_v_ar = realloc(t->par_v_ar, (t->mpar+100)*sizeof(int)));
      memset(&t->par_v_ar[t->mpar], 0, 100*sizeof(int));
      t->mpar += 100;
      }
      t->npar++;
      t->par_v_ar[t->cur_par] = par_v;
      }
    */
  }
  if(t->meta->grp_col == -1) {
    gid = 0;
  } else {
    gid = ts_symbol_id_factory_retrieve_id(t->idf, *((char***)data_ar)[t->meta->grp_col]);
  }
  TS_TRY(gid >= 0);

  if(t->cur_nelem == t->cur_melem) {
    ts_realloc(t->cur_seq_ar, t->cur_melem, 2000, int);
    t->cur_melem += 2000;
  }
  t->cur_seq_ar[t->cur_nelem] = gid;
  t->cur_nelem++;

  if(gid == t->ngrp) { // new group
    ((char**)t->grp_v_ar)[gid] = t->idf->symbol_ar[gid];
    if(t->ngrp == t->mgrp) { // new mem for new group
      TS_TRY(t->mrow_ar = realloc(t->mrow_ar, (t->mgrp + 1000)*sizeof(int)));
      for(i=t->mgrp;i<t->mgrp+1000;i++) {
	t->mrow_ar[i] = 1000;
      }
      TS_TRY(t->nrow_ar = realloc(t->nrow_ar, (t->mgrp + 1000)*sizeof(int)));
      memset(&t->nrow_ar[t->mgrp], 0, 1000*sizeof(int));
      TS_TRY(t->grp_v_ar = realloc(t->grp_v_ar, (t->mgrp + 1000)*sizeof(void*)));
      memset(&((int*)t->grp_v_ar)[t->mgrp], 0, 1000*sizeof(void*));
      TS_TRY(t->cur_par_start_ar = realloc(t->cur_par_start_ar, (t->mgrp + 1000)*sizeof(int)));
      memset(&t->cur_par_start_ar[t->mgrp], 0, 1000*sizeof(int));
      TS_TRY(t->cur_row_ar = realloc(t->cur_row_ar, (t->mgrp + 1000)*sizeof(int)));
      memset(&t->cur_row_ar[t->mgrp], 0, 1000*sizeof(int));

      for(i=0;i<t->meta->ncol;i++) {
	if(i != t->meta->grp_col) {
	  t->p_col_ar[i] = realloc(t->p_col_ar[i], (t->mgrp + 1000)*sizeof(void*));
	  memset(t->p_col_ar[i]+t->mgrp, 0, 1000*sizeof(void*));
	}
      }
      for(i=t->meta->ncol;i<t->nttlcol;i++) {
	t->var_col_ar[i] = realloc(t->var_col_ar[i], (t->mgrp + 1000)*sizeof(void*));
	memset(t->var_col_ar[i]+t->mgrp, 0, 1000*sizeof(void*));
      }
      t->mgrp += 1000;
    }
    for(i=0;i<t->meta->ncol;i++) {
      if(i != t->meta->grp_col) {
	TS_TRY(t->p_col_ar[i][gid] = calloc(t->mrow_ar[gid], t->meta->size_ar[i]));
      }
    }
    t->ngrp++;
  }
  rowidx = t->nrow_ar[gid];
  if(rowidx == t->mrow_ar[gid]) {
    for(i=0;i<t->meta->ncol;i++) {
      if(i != t->meta->grp_col) {
	TS_TRY(t->p_col_ar[i][gid] = realloc(t->p_col_ar[i][gid], (t->mrow_ar[gid]*2)*t->meta->size_ar[i]));
	memset(((char*)t->p_col_ar[i][gid])+t->meta->size_ar[i]*t->mrow_ar[gid], 0, t->mrow_ar[gid]*t->meta->size_ar[i]);
      }
    }
    for(i=t->meta->ncol;i<t->nttlcol;i++) {
      TS_TRY(t->var_col_ar[i][gid] = realloc(t->var_col_ar[i][gid], (t->mrow_ar[gid]*2)*t->var_size_ar[i]));
      memset(((char*)t->var_col_ar[i][gid])+t->var_size_ar[i]*t->mrow_ar[gid], 0, t->mrow_ar[gid]*t->var_size_ar[i]);
    }
    t->mrow_ar[gid] += t->mrow_ar[gid];
  }
  for(i=0;i<t->meta->ncol;i++) {
    if(i == t->meta->grp_col)
      continue;
    switch(t->meta->type_ar[i]) {
    case TSTYPE_INT:
      ((int*)t->p_col_ar[i][gid])[rowidx] = *((int*)data_ar[i]);
      break;
    case TSTYPE_FLOAT:
      ((float*)t->p_col_ar[i][gid])[rowidx] = *((float*)data_ar[i]);
      break;
    case TSTYPE_DOUBLE:
      ((double*)t->p_col_ar[i][gid])[rowidx] = *((double*)data_ar[i]);
      break;
    case TSTYPE_TIME:
      ((ts_time_t*)t->p_col_ar[i][gid])[rowidx] = *((ts_time_t**)data_ar)[i];
      break;
    default:
      (void)ts_warn(stderr, "\n");
      goto fail;
    }
  }
  t->nrow_ar[gid]++;
  return 1;
 fail:
  return 0;
}


int ts_table_write_cur_partition(ts_table_t *t)
{
  TS_TRY(t && t->cur_par >= 0);
  char buf[1000];
  FILE *fp;
  int i, j, rv;

  if(t->cur_par == 0) {
    TS_TRY(ts_table_write_meta(t));
  }
  snprintf(buf, 1000, "%s/%d", t->meta->tablename, t->par_v_ar[t->cur_par]);
  rv = mkdir(buf, S_IRWXU);
  TS_TRY(rv == 0 || errno == EEXIST);
  
  for(i=0;i<t->meta->ncol;i++) {
    if(i != t->meta->grp_col) {
      snprintf(buf, 1000, "%s/%d/%s", t->meta->tablename, t->par_v_ar[t->cur_par], t->meta->colname_ar[i]);
      TS_TRY(fp = fopen(buf, "wb"));
      for(j=0;j<t->ngrp;j++) {
	fwrite(t->p_col_ar[i][j]+(t->cur_par_start_ar[j]*t->meta->size_ar[i]), t->meta->size_ar[i], t->nrow_ar[j] - t->cur_par_start_ar[j], fp);
	if(t->nlag > 0) {
	  if(t->nrow_ar[j] > t->nlag) {
	    memmove(t->p_col_ar[i][j], t->p_col_ar[i][j]+(t->nrow_ar[j]-t->nlag)*t->meta->size_ar[i], t->nlag*t->meta->size_ar[i]);
	  }
	}
      }
      fclose(fp);
    }
  }

  snprintf(buf, 1000, "%s/%d/.grp", t->meta->tablename, t->par_v_ar[t->cur_par]);
  TS_TRY(fp = fopen(buf, "wb"));
  for(i=0,j=0;i<t->ngrp;i++) {
    TS_TRY(t->cur_par_start_ar[i] <= t->nrow_ar[i]);
    j += t->nrow_ar[i] - t->cur_par_start_ar[i];
    fwrite(&j, sizeof(int), 1, fp);
  }
  fclose(fp);
  
  for(j=0;j<t->ngrp;j++) {
    t->cur_par_start_ar[j] = min(t->nlag, t->nrow_ar[j]);
    t->nrow_ar[j] = t->cur_par_start_ar[j];
  }

  snprintf(buf, 1000, "%s/.idf", t->meta->tablename);
  TS_TRY(fp = fopen(buf, "w"));
  TS_TRY(ts_symbol_id_factory_save(t->idf, fp));
  fclose(fp);

  snprintf(buf, 1000, "%s/%d/.seq", t->meta->tablename, t->par_v_ar[t->cur_par]);
  TS_TRY(fp = fopen(buf, "wb"));
  fwrite(t->cur_seq_ar, sizeof(int), t->cur_nelem, fp);
  fclose(fp);
  t->cur_nelem = 0;
  
  return 1;
 fail:
  return 0;
}


int ts_table_write(ts_table_t *t)
{
  TS_TRY(t);
  int i,j;
  char buf[1000];
  FILE *fp;

  if(t->meta->par_col == -1) {
    TS_TRY(ts_table_write_meta(t));
  
    for(i=0;i<t->meta->ncol;i++) {
      if(i != t->meta->grp_col) {
	snprintf(buf, 1000, "%s/%s", t->meta->tablename, t->meta->colname_ar[i]);
	TS_TRY(fp = fopen(buf, "wb"));
	for(j=0;j<t->ngrp;j++) {
	  fwrite(t->p_col_ar[i][j], t->meta->size_ar[i], t->nrow_ar[j], fp);
	}
	fclose(fp);
      }
    }

    snprintf(buf, 1000, "%s/.idf", t->meta->tablename);
    TS_TRY(fp = fopen(buf, "w"));
    TS_TRY(ts_symbol_id_factory_save(t->idf, fp));
    fclose(fp);

    snprintf(buf, 1000, "%s/.grp", t->meta->tablename);
    TS_TRY(fp = fopen(buf, "wb"));
    for(i=0,j=0;i<t->ngrp;i++) {
      j += t->nrow_ar[i];
      fwrite(&j, sizeof(int), 1, fp);
    }
    fclose(fp);

    snprintf(buf, 1000, "%s/.seq", t->meta->tablename);
    TS_TRY(fp = fopen(buf, "wb"));
    fwrite(t->cur_seq_ar, sizeof(int), t->cur_nelem, fp);
    fclose(fp);

  } else { //partitioned table
    TS_TRY(ts_table_write_cur_partition(t));
  }
  
  t->dirty = 0;
  
  return 1;
 fail:
  return 0;
}

ts_table_meta_t *ts_table_read_meta(const char *tablename)
{
  TS_TRY(tablename);
  ts_table_meta_t *meta;
  FILE *mfp;
  char buf[BUFSIZ];
  char *str, *v;
  int sz,i,type,l;
  TS_TRY(meta = calloc(1, sizeof(ts_table_meta_t)));
  TS_TRY(meta->tablename = strdup(tablename));
  snprintf(buf, 1000, "%s/.m", tablename);
  mfp = fopen(buf, "rb");
  if(!mfp) {
    (void)ts_warn(stderr, "runtime error @[%d|%d]: can not find table [%s]'s metafile.\n", vm->l, vm->c, tablename);
    goto fail;
  }
  memset(buf, 0, BUFSIZ);
  sz = fread(buf, sizeof(char), BUFSIZ, mfp);
  
  str = buf;
  meta->ncol = *((int*)str);
  meta->par_col = *((int*)(str+4));
  meta->grp_col = *((int*)(str+8));
  meta->sub_par_by = *((int*)(str+12));
  TS_TRY(meta->colname_ar = calloc(meta->ncol, sizeof(char*)));
  TS_TRY(meta->type_ar = calloc(meta->ncol, sizeof(ts_type_t)));
  TS_TRY(meta->size_ar = calloc(meta->ncol, sizeof(int)));
  
  str += 16;
  sz -= 16;
  i = 0;
  while(*str) {
    v = memchr(str, '\0', sz);
    if(!v)
      break;
    TS_TRY(meta->colname_ar[i] = strdup(str));
    sz -= v-str+1;
    str = v+1;
    
    type = atoi(str);
    l = strlen(str) + 1;
    meta->type_ar[i] = type;
    switch(type) {
    case TSTYPE_INT:
      meta->size_ar[i] = sizeof(int);
      break;
    case TSTYPE_FLOAT:
      meta->size_ar[i] = sizeof(float);
      break;
    case TSTYPE_DOUBLE:
      meta->size_ar[i] = sizeof(double);
      break;
    case TSTYPE_SYM:
      meta->size_ar[i] = sizeof(int);
      break;
    case TSTYPE_TIME:
      meta->size_ar[i] = sizeof(ts_time_t);
      break;
    default:
      (void)ts_warn(stderr, "\n");
      goto fail;
    }
    i++;
    sz -= l;
    str += l;
  }

  TS_TRY(i == meta->ncol);

  return meta;
 fail:
  return 0;
}

ts_table_t *ts_table_read(const char *tablename)
{
  TS_TRY(tablename);
  DIR *dir; //for traversal dir
  struct dirent *dirent;
  int i, j, ngrp;
  FILE *fp;
  char buf[1000];
  ts_table_meta_t *meta;
  ts_table_t *t;
  size_t sz;
  int *grp_end_idx_ar = 0;
  
  TS_TRY(meta = ts_table_read_meta(tablename));
  TS_TRY(t = calloc(1, sizeof(ts_table_t)));
  t->meta = meta;
  TS_TRY(ts_table_copy_meta_to_var(t));
  t->nlag = 100;
  for(i=0;i<t->meta->ncol;i++) {
    if(t->meta->type_ar[i] == TSTYPE_TIME) {
      t->tcol = i;
      break;
    }
  }

  if(t->meta->par_col == -1) { // not partitioned
    t->npar = 1;
    t->mpar = 1;
    t->cur_par = 0;
    snprintf(buf, 1000, "%s/.grp", tablename);
    TS_TRY(ts_file_size(buf, &sz));
    TS_TRY(sz % sizeof(int) == 0);
    ngrp = sz / sizeof(int);
    TS_TRY(fp = fopen(buf, "rb"));
    TS_TRY(grp_end_idx_ar = malloc(sz));
    fread(grp_end_idx_ar, sz, 1, fp);
    fclose(fp);

    snprintf(buf, 1000, "%s/.idf", tablename);
    TS_TRY(t->idf = ts_symbol_id_factory_new(1000, buf, TS_SYMBOL_ID_FACTORY_MODE_READ));
    TS_TRY(t->idf->nentry == ngrp || (t->idf->nentry == 0 && ngrp == 1 && t->meta->grp_col == -1));

    t->mgrp = ngrp;
    t->ngrp = ngrp;
    TS_TRY(t->mrow_ar = malloc(t->mgrp * sizeof(int)));
    TS_TRY(t->nrow_ar = malloc(t->mgrp * sizeof(int)));
    TS_TRY(t->grp_v_ar = calloc(t->mgrp, sizeof(void*)));
    TS_TRY(t->cur_row_ar = calloc(t->mgrp, sizeof(int)));
    
    t->nrow_ar[0] = grp_end_idx_ar[0];
    t->mrow_ar[0] = t->nrow_ar[0];
    for(j=1;j<ngrp;j++) {
      t->nrow_ar[j] = grp_end_idx_ar[j] - grp_end_idx_ar[j-1];
      t->mrow_ar[j] = t->nrow_ar[j];
      ((char**)t->grp_v_ar)[j] = t->idf->symbol_ar[j];
    }

    t->cur_nelem = grp_end_idx_ar[ngrp-1];
    if(t->cur_melem < t->cur_nelem) {
      TS_TRY(t->cur_seq_ar = realloc(t->cur_seq_ar, t->cur_nelem*sizeof(int)));
      memset(&t->cur_seq_ar[t->cur_melem], 0, (t->cur_nelem-t->cur_melem)*sizeof(int));
      t->cur_melem = t->cur_nelem;
    }
    snprintf(buf, 1000, "%s/.seq", tablename);
    TS_TRY(fp = fopen(buf, "rb"));
    sz = fread(t->cur_seq_ar, sizeof(int), t->cur_nelem, fp);
    TS_TRY(sz == t->cur_nelem);
    fclose(fp);
    
    TS_TRY(t->p_col_ar = calloc(meta->ncol, sizeof(void**)));

    for(i=0;i<meta->ncol;i++) {
      if(i != meta->grp_col) {
	TS_TRY(t->p_col_ar[i] = calloc(t->mgrp, sizeof(void*)));
	for(j=0;j<ngrp;j++) {
	  TS_TRY(t->p_col_ar[i][j] = malloc(t->mrow_ar[j] * meta->size_ar[i]));
	}
      }
    }
    
    for(i=0;i<meta->ncol;i++) {
      if(i != meta->grp_col) {
	snprintf(buf, 1000, "%s/%s", tablename, meta->colname_ar[i]);
	TS_TRY(fp = fopen(buf, "rb"));
	for(j=0;j<ngrp;j++) {
	  fread(t->p_col_ar[i][j], meta->size_ar[i], t->nrow_ar[j], fp);
	}
	fclose(fp);
      } else {
	t->p_col_ar[i] = 0;
      }
    }
  } else if(t->meta->par_col >= 0) {
    snprintf(buf, 1000, "%s/.idf", tablename);
    TS_TRY(t->idf = ts_symbol_id_factory_new(1000, buf, TS_SYMBOL_ID_FACTORY_MODE_READ));

    ngrp = t->idf->nentry == 0 ? 1 : t->idf->nentry;
    t->mgrp = ngrp;
    t->ngrp = ngrp;
    TS_TRY(t->mrow_ar = calloc(t->mgrp, sizeof(int)));
    TS_TRY(t->nrow_ar = calloc(t->mgrp, sizeof(int)));
    TS_TRY(t->grp_v_ar = calloc(t->mgrp, sizeof(void*)));
    TS_TRY(t->cur_row_ar = calloc(t->mgrp, sizeof(int)));

    snprintf(buf, 1000, "%s", tablename);
    TS_TRY(dir = opendir(buf));
    int ndir = 0;
    char *p;
    int mdir = 10, *dir_ar = calloc(mdir, sizeof(int));
    TS_TRY(dir_ar);
    while(0 != (dirent = readdir(dir))) {
      dir_ar[ndir] = strtol(dirent->d_name, &p, 10);
      if(*p == '\0') {
	ndir++;
	if(ndir == mdir) {
	  TS_TRY(dir_ar = realloc(dir_ar, (mdir + 10)*sizeof(int)));
	  memset(&dir_ar[mdir], 0, 10*sizeof(int));
	  mdir += 10;
	}
      }
    }
    if(ndir <= 0) {
      (void)ts_warn(stderr, "runtime error @[%d|%d]: no partition.\n", vm->l, vm->c);
      goto fail;
    }
    qsort(dir_ar, ndir, sizeof(int), intcmp);
    closedir(dir);
    t->npar = ndir;
    t->mpar = ndir + 10;
    TS_TRY(t->par_v_ar = calloc(t->mpar, sizeof(int)));
    TS_TRY(t->cur_par_start_ar = calloc(t->mgrp, sizeof(int)));
    for(i=0;i<t->npar;i++) {
      t->par_v_ar[i] = dir_ar[i];
    }

    TS_TRY(t->p_col_ar = calloc(meta->ncol, sizeof(void**)));

    for(i=0;i<meta->ncol;i++) {
      if(i != meta->grp_col) {
	TS_TRY(t->p_col_ar[i] = calloc(t->mgrp, sizeof(void*)));
      }
    }
    TS_TRY(_ts_table_partition_read(t, 0));
    t->cur_par = 0;
  }
  if(grp_end_idx_ar) {
    free(grp_end_idx_ar);
  }
  
  TS_TRY(t->var_mgr = ts_var_mgr_new(t->ngrp));

  return t;
 fail:
  return 0;
}

int _ts_table_partition_read(ts_table_t *t, int npar)
{
  TS_TRY(t && npar >= 0 && npar < t->npar);
  int i,j, ngrp;
  char buf[1000];
  size_t sz;
  int *grp_end_idx_ar;
  FILE *fp;
  int *old_mrow_ar;
  int *start_ar;

  snprintf(buf, 1000, "%s/%d/.grp", t->meta->tablename, t->par_v_ar[npar]);
  TS_TRY(ts_file_size(buf, &sz));
  TS_TRY(sz % sizeof(int) == 0);
  ngrp = sz / sizeof(int);
  TS_TRY(fp = fopen(buf, "rb"));
  TS_TRY(grp_end_idx_ar = alloca(sz));
  fread(grp_end_idx_ar, sz, 1, fp);
  fclose(fp);

  t->cur_nelem = grp_end_idx_ar[ngrp-1];
  if(t->cur_melem < t->cur_nelem) {
    TS_TRY(t->cur_seq_ar = realloc(t->cur_seq_ar, t->cur_nelem*sizeof(int)));
    memset(&t->cur_seq_ar[t->cur_melem], 0, (t->cur_nelem-t->cur_melem)*sizeof(int));
    t->cur_melem = t->cur_nelem;
  }
  snprintf(buf, 1000, "%s/%d/.seq", t->meta->tablename, t->par_v_ar[npar]);
  TS_TRY(fp = fopen(buf, "rb"));
  sz = fread(t->cur_seq_ar, sizeof(int), t->cur_nelem, fp);
  TS_TRY(sz == t->cur_nelem);
  fclose(fp);
  
  start_ar = alloca(sizeof(int)*ngrp);
  for(j=0;j<ngrp;j++) {
    start_ar[j] = t->nrow_ar[j] < t->nlag ? t->nrow_ar[j] : t->nlag;
    if(start_ar[j] < t->nrow_ar[j]) {
      for(i=0;i<t->meta->ncol;i++) {
	if(i != t->meta->grp_col) {
	  memmove(t->p_col_ar[i][j], t->p_col_ar[i][j]+(t->nrow_ar[j]-t->nlag)*t->meta->size_ar[i], t->nlag*t->meta->size_ar[i]);
	}
      }
      for(i=t->meta->ncol;i<t->nttlcol;i++) {
	memmove(t->var_col_ar[i][j], t->var_col_ar[i][j]+(t->nrow_ar[j]-t->nlag)*t->var_size_ar[i], t->nlag*t->var_size_ar[i]);
      }
    }
  }

  old_mrow_ar = alloca(sizeof(int)*ngrp);
  t->nrow_ar[0] = grp_end_idx_ar[0];
  old_mrow_ar[0] = t->mrow_ar[0];
  t->mrow_ar[0] = max(t->mrow_ar[0], t->nrow_ar[0]+t->nlag);
  for(j=1;j<ngrp;j++) {
    t->nrow_ar[j] = grp_end_idx_ar[j] - grp_end_idx_ar[j-1];
    old_mrow_ar[j] = t->mrow_ar[j];
    t->mrow_ar[j] = max(t->nrow_ar[j]+t->nlag, t->mrow_ar[j]);
    TS_TRY(t->nrow_ar[j] >= 0);
  }

  for(i=0;i<t->meta->ncol;i++) {
    if(i != t->meta->grp_col) {
      for(j=0;j<ngrp;j++) {
	if(t->mrow_ar[j] > old_mrow_ar[j]) {
	  TS_TRY(t->p_col_ar[i][j] = realloc(t->p_col_ar[i][j], t->mrow_ar[j] * t->meta->size_ar[i]));
	}
      }
    }
  }
  for(i=t->meta->ncol;i<t->nttlcol;i++) {
    for(j=0;j<ngrp;j++) {
      if(t->mrow_ar[j] > old_mrow_ar[j]) {
	TS_TRY(t->var_col_ar[i][j] = realloc(t->var_col_ar[i][j], t->mrow_ar[j] * t->var_size_ar[i]));
      }
    }
  }
  for(i=0;i<t->meta->ncol;i++) {
    if(i != t->meta->grp_col) {
      snprintf(buf, 1000, "%s/%d/%s", t->meta->tablename, t->par_v_ar[npar], t->meta->colname_ar[i]);
      TS_TRY(fp = fopen(buf, "rb"));
      for(j=0;j<ngrp;j++) {
	TS_TRY(t->mrow_ar[j] * t->meta->size_ar[i] > 0);
	fread(t->p_col_ar[i][j] + (t->meta->size_ar[i]*start_ar[j]), t->meta->size_ar[i], t->nrow_ar[j], fp);
	t->cur_row_ar[j] = start_ar[j];
	t->cur_par_start_ar[j] = start_ar[j];
      }
      fclose(fp);
    } else {
      t->p_col_ar[i] = 0;
    }
  }
  for(j=0;j<ngrp;j++) {
    t->nrow_ar[j] += start_ar[j];
  }

  return 1;
 fail:
  return 0;
}

int ts_table_partition_reset(ts_table_t *t)
{
  TS_TRY(t);
  int i;
  if(t->meta->par_col == -1) {
    for(i=0;i<t->ngrp;i++) {
      t->cur_row_ar[i] = 0;
    }
  } else if(t->cur_par != 0) {
    TS_TRY(_ts_table_partition_read(t, 0));
    t->cur_par = 0;
  }
  return 1;
 fail:
  return 0;
}

int ts_table_partition_next(ts_table_t *t)
{
  TS_TRY(t);
  if(t->cur_par != t->npar-1) {
    TS_TRY(_ts_table_partition_read(t, ++t->cur_par));
  } else {
    return -1;
  }
  return 1;
 fail:
  return 0;
}

int ts_table_cur_var_print(ts_table_t *t, int gid, int offset)
{
  TS_TRY(t);
  int i=0;
  
  if(t->idf->symbol_ar[gid])
    printf("%s|", t->idf->symbol_ar[gid]);
  else
    printf("%s|", t->meta->tablename);
  
  for(i=t->meta->ncol;i<t->nttlcol;i++) {
    if(t->is_out_var_ar[i]) {
      printf("\t");
      switch(t->var_type_ar[i]) {
      case TSTYPE_INT:
	printf("%d", ((int*)t->var_col_ar[i][gid])[t->cur_row_ar[gid]+offset]);
	break;
      case TSTYPE_FLOAT:
	printf("%f", ((float*)t->var_col_ar[i][gid])[t->cur_row_ar[gid]+offset]);
	break;
      case TSTYPE_DOUBLE:
	printf("%lf", ((double*)t->var_col_ar[i][gid])[t->cur_row_ar[gid]+offset]);
	break;
      case TSTYPE_TIME:
	printf("%s", ts_time_to_str(((ts_time_t*)t->var_col_ar[i][gid])[t->cur_row_ar[gid]+offset]));
	break;
      default:
	(void)ts_warn(stderr, "\n");
	goto fail;
      }
    }
  }
  //printf(" %d \n", t->cur_row_ar[gid]);
  printf("\n");
  return 1;
 fail:
  return 0;
}

int ts_table_get_time_par_v(ts_table_meta_t *meta, ts_time_t t)
{
  TS_TRY(meta && t && meta->sub_par_by);
  
  switch(meta->sub_par_by) {
  case 1:
    return ts_time_get_day(t);
  case 2:
    return ts_time_get_month(t);
  default:    
    return 0;
  }
 fail:
  return 0;
}

/*
  int ts_table_set_start(ts_table_t *t, int idx, int value)
  {
  TS_TRY(t && idx >= 0 && idx < t->meta->ncol);
  int i, j;
  for(i=0;i<t->ngrp;i++) {
  for(j = t->cur_row_ar[i];j<t->nrow_ar[i];j++) {
  if(((int*)(t->p_col_ar[idx][i]))[j] > value) {
  break;
  }
  }
  t->cur_row_ar[i] = j-1;
  }

  return 1;
  fail:
  return 0;
  }
*/
int ts_table_new_var(ts_table_t *t, const char *varname, ts_type_t type, int is_out, short *result)
{
  TS_TRY(t && result);
  int i;
  short varid;
  int *id;
  id = ts_hash_table_get(t->var_table, varname);
  if( id ) {
    *result = *id;
    return 1;
  }
  
  varid = t->nttlcol;
  if(t->nttlcol == t->mttlcol) {
    TS_TRY(t->varname_ar = realloc(t->varname_ar, (t->mttlcol+10)*sizeof(char*)));
    memset(&t->varname_ar[t->mttlcol], 0, sizeof(char*));
    TS_TRY(t->var_col_ar = realloc(t->var_col_ar, (t->mttlcol+10)*sizeof(void**)));
    memset(&t->var_col_ar[t->mttlcol], 0, sizeof(void**));
    TS_TRY(t->var_type_ar = realloc(t->var_type_ar, (t->mttlcol+10)*sizeof(ts_type_t)));
    memset(&t->var_type_ar[t->mttlcol], 0, sizeof(ts_type_t));
    TS_TRY(t->var_size_ar = realloc(t->var_size_ar, (t->mttlcol+10)*sizeof(int)));
    memset(&t->var_size_ar[t->mttlcol], 0, sizeof(int));
    TS_TRY(t->is_out_var_ar = realloc(t->is_out_var_ar, (t->mttlcol+10)*sizeof(int)));
    memset(&t->is_out_var_ar[t->mttlcol], 0, sizeof(int));
    t->mttlcol += 10;
  }
  t->nttlcol++;

  t->is_out_var_ar[varid] = is_out;
  t->var_type_ar[varid] = type;
  t->varname_ar[varid] = strdup(varname);
  switch(type) {
  case TSTYPE_INT:
    t->var_size_ar[varid] = sizeof(int);
    break;
  case TSTYPE_FLOAT:
    t->var_size_ar[varid] = sizeof(float);
    break;
  case TSTYPE_DOUBLE:
    t->var_size_ar[varid] = sizeof(double);
    break;
  case TSTYPE_SYM:
    t->var_size_ar[varid] = sizeof(int);
    break;
  case TSTYPE_TIME:
    t->var_size_ar[varid] = sizeof(ts_time_t);
    break;
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  TS_TRY(t->var_col_ar[varid] = calloc(t->mgrp, sizeof(void*)));
  for(i=0;i<t->mgrp;i++) {
    TS_TRY(t->var_col_ar[varid][i] = calloc(t->mrow_ar[i], t->var_size_ar[varid]));
  }

  TS_TRY(ts_hash_table_insert(t->var_table, varname, &varid));
  *result = varid;
  return 1;
 fail:
  return 0;
}

inline int ts_table_set_vari(ts_table_t *t, short varid, int gid, int v)
{
  TS_TRY(t && varid >= 0 && varid < t->nttlcol && gid >= 0);
  int *col = varid < t->meta->ncol ? (int*)t->p_col_ar[varid][gid] : (int*)t->var_col_ar[varid][gid];
  col[t->cur_row_ar[gid]] = v;
  return 1;
 fail:
  return 0;
}

inline int ts_table_set_varf(ts_table_t *t, short varid, int gid, float v)
{
  TS_TRY(t && varid >= 0 && varid < t->nttlcol && gid >= 0);
  float *col = varid < t->meta->ncol ? (float*)t->p_col_ar[varid][gid] : (float*)t->var_col_ar[varid][gid];
  col[t->cur_row_ar[gid]] = v;
  return 1;
 fail:
  return 0;
}

inline int ts_table_set_varlf(ts_table_t *t, short varid, int gid, double v)
{
  TS_TRY(t && varid >= 0 && varid < t->nttlcol && gid >= 0);
  double *col = varid < t->meta->ncol ? (double*)t->p_col_ar[varid][gid] : (double*)t->var_col_ar[varid][gid];
  col[t->cur_row_ar[gid]] = v;
  return 1;
 fail:
  return 0;
}


inline int ts_table_get_vari(ts_table_t *t, short varid, int gid, int *v)
{
  TS_TRY(t && varid >= 0 && gid >= 0 && v);
  int *col = varid < t->meta->ncol ? (int*)t->p_col_ar[varid][gid] : (int*)t->var_col_ar[varid][gid];
  *v = col[t->cur_row_ar[gid]];
  return 1;
 fail:
  return 0;
}

inline int ts_table_get_varf(ts_table_t *t, short varid, int gid, float *v)
{
  TS_TRY(t && varid >= 0 && gid >= 0 && v);
  float *col = varid < t->meta->ncol ? (float*)t->p_col_ar[varid][gid] : (float*)t->var_col_ar[varid][gid];
  *v = col[t->cur_row_ar[gid]];
  return 1;
 fail:
  return 0;
}

inline int ts_table_get_varlf(ts_table_t *t, short varid, int gid, double *v)
{
  TS_TRY(t && varid >= 0 && gid >= 0 && v);
  double *col = varid < t->meta->ncol ? (double*)t->p_col_ar[varid][gid] : (double*)t->var_col_ar[varid][gid];
  *v = col[t->cur_row_ar[gid]];
  return 1;
 fail:
  return 0;
}

inline int ts_table_get_varsym(ts_table_t *t, short varid, int gid, char **v)
{
  TS_TRY(t && varid >= 0 && gid >= 0 && v);
  TS_TRY(t->meta->type_ar[varid] == TSTYPE_SYM);
  *v = ts_symbol_id_factory_get_symbol(t->idf, gid);
  return 1;
 fail:
  return 0;
}

inline int ts_table_get_vari_ref(ts_table_t *t, short varid, int gid, int offset, int *v)
{
  TS_TRY(t && varid >= 0 && gid >= 0 && offset >= 0 && v);
  int idx = t->cur_row_ar[gid] - offset;
  idx = idx >= 0 ? idx : 0; 
  int *col = varid < t->meta->ncol ? (int*)t->p_col_ar[varid][gid] : (int*)t->var_col_ar[varid][gid];
  *v = col[idx];
  
  return 1;
 fail:
  return 0;
}

inline int ts_table_get_varf_ref(ts_table_t *t, short varid, int gid, int offset, float *v)
{
  TS_TRY(t && varid >= 0 && gid >= 0 && offset >= 0 && v);
  int idx = t->cur_row_ar[gid] - offset;
  idx = idx >= 0 ? idx : 0;
  float *col = varid < t->meta->ncol ? (float*)t->p_col_ar[varid][gid] : (float*)t->var_col_ar[varid][gid];
  *v = col[idx];  
  
  return 1;
 fail:
  return 0;
}

inline int ts_table_get_varlf_ref(ts_table_t *t, short varid, int gid, int offset, double *v)
{
  TS_TRY(t && varid >= 0 && gid >= 0 && offset >= 0 && v);
  int idx = t->cur_row_ar[gid] - offset;
  idx = idx >= 0 ? idx : 0;
  double *col = varid < t->meta->ncol ? (double*)t->p_col_ar[varid][gid] : (double*)t->var_col_ar[varid][gid];
  *v = col[idx];
  
  return 1;
 fail:
  return 0;
}


ts_type_t ts_table_get_var_type(ts_table_t *t, short varid)
{
  TS_TRY(t && varid >= 0);
  ts_type_t *type_ar = varid < t->meta->ncol ? t->meta->type_ar : t->var_type_ar;
  if(varid < t->nttlcol)
    return type_ar[varid];
  else
    return TSTYPE_NONE;
 fail:
  return 0;
}

short ts_table_get_var_id(ts_table_t *t, const char *varname)
{
  TS_TRY(t && varname);
  int *ptr;
  ptr = ts_hash_table_get(t->var_table, varname);
  if(!ptr) {
    return -1;
  }
  return *ptr;
 fail:
  return -1;
}

int ts_table_copy_meta_to_var(ts_table_t *t)
{
  TS_TRY(t && t->meta);
  int i;
  t->mttlcol = t->meta->ncol + 10;
  t->nttlcol = t->meta->ncol;
  TS_TRY(t->var_table = ts_hash_table_new(1000, sizeof(int)));
  TS_TRY(t->varname_ar = calloc(t->mttlcol, sizeof(char*)));
  TS_TRY(t->var_type_ar = calloc(t->mttlcol, sizeof(ts_type_t)));
  TS_TRY(t->is_out_var_ar = calloc(t->mttlcol, sizeof(int)));
  TS_TRY(t->var_size_ar = calloc(t->mttlcol, sizeof(int)));
  TS_TRY(t->var_col_ar = calloc(t->mttlcol, sizeof(void**)));

  for(i=0;i<t->meta->ncol;i++) {
    TS_TRY(ts_hash_table_insert(t->var_table, t->meta->colname_ar[i], &i));
    t->varname_ar[i] = strdup(t->meta->colname_ar[i]);
    t->var_type_ar[i] = t->meta->type_ar[i];
    t->is_out_var_ar[i] = 0;
    t->var_size_ar[i] = t->meta->size_ar[i];
  }
  return 1;
 fail:
  return 0;
}

int ts_table_copy_previous_var_part(ts_table_t *t, int gid, int nvar, int *varid_ar)
{
  TS_TRY(t && gid >= 0);
  int i;
  int preidx = t->cur_row_ar[gid]-1;
  if(preidx>=0) {
    for(i=0;i<nvar;i++) {
      int varid = varid_ar[i];
      memcpy(((char*)(t->var_col_ar[varid][gid]))+t->var_size_ar[varid]*t->cur_row_ar[gid], 
	     ((char*)(t->var_col_ar[varid][gid]))+t->var_size_ar[varid]*preidx, t->var_size_ar[varid]);
    }
  }
  return 1;
 fail:
  return 0;
}

int ts_table_copy_previous_var_all(ts_table_t *t, int gid)
{
  TS_TRY(t && gid >= 0);
  int i;
  int preidx = t->cur_row_ar[gid]-1;
  if(preidx>=0) {
    for(i=t->meta->ncol;i<t->nttlcol;i++) {
      memcpy(((char*)(t->var_col_ar[i][gid]))+t->var_size_ar[i]*t->cur_row_ar[gid], 
	     ((char*)(t->var_col_ar[i][gid]))+t->var_size_ar[i]*preidx, t->var_size_ar[i]);
    }
  }
  return 1;
 fail:
  return 0;
}

unsigned ts_table_translate_gid(ts_table_t *from, ts_table_t *to, unsigned gid)
{
  TS_TRY(from && to && gid >= 0);
  unsigned id;
  if(from->idf->nentry == 0) {
    TS_TRY(to->idf->nentry == 0);
    id = 0;
  } else {
    unsigned *ptr = (unsigned*)ts_symbol_id_factory_get_id(to->idf, from->idf->symbol_ar[gid]);
    if(ptr) {
      id = *ptr;
    } else {
      (void)ts_warn(stderr, "runtime error @[%d|%d]: can not find symbol:[%s] in another table.\n", vm->l, vm->c, from->idf->symbol_ar[gid]);
      goto fail;
    }
  }
  return id;
 fail:
  return 0;
}

int ts_table_sum(ts_table_t *t, short varid, int gid, int length, ts_sum_store_t *st, ts_type_t rvt, void *v)
{
  TS_TRY(t && varid >= 0 && gid >= 0 && length >= 0 && st && v);
  int i=0;
  double s = 0;
  int c = t->cur_row_ar[gid];
  int start = length == 0 ? 0 : c-length+1;
  void *col;
  ts_type_t vt;							
  if(varid < t->meta->ncol) {
    col = t->p_col_ar[varid][gid];
    vt = t->meta->type_ar[varid];
  } else {
    col = t->var_col_ar[varid][gid];
    vt = t->var_type_ar[varid];
  }
  if(start < 0) {
    start = 0;
  }
  switch(vt) {
  case TSTYPE_INT:
    for(i=start;i<=c;i++){
      s += ((int*)col)[i];
    }
    break;								
  case TSTYPE_FLOAT:							
    for(i=start;i<=c;i++){
      if(!isnan(((float*)col)[i]))
	s += ((float*)col)[i];
    }									
    break;								
  case TSTYPE_DOUBLE:							
    for(i=start;i<=c;i++){
      if(!isnan(((float*)col)[i]))
	s += ((double*)col)[i];						
    }									
    break;								
  default:								
    (void)ts_warn(stderr, "runtime error @[%d|%d]: sum var type error.\n", vm->l, vm->c);
    goto fail;								
  }
  switch(rvt) {
  case TSTYPE_INT:
    *(int*)v = s;
    break;
  case TSTYPE_FLOAT:
    *(float*)v = s;
    break;
  case TSTYPE_DOUBLE:
    *(double*)v = s;
    break;
  default:
    (void)ts_warn(stderr, "runtime error @[%d|%d]: sum return type error.\n", vm->l, vm->c);	
    goto fail;
  }
  return 1;								
 fail:									
  return 0;								
}									

int ts_table_sma(ts_table_t *t, short varid, int gid, int length, ts_sma_store_t *st, ts_type_t rvt, void *v) 
{
  TS_TRY(t && varid >=0 && gid >=0 && length > 0 && v);
  double s = 0.0;
  int c = t->cur_row_ar[gid];
  int start = c - length + 1, i;
  void *col;
  ts_type_t vt;	
  if(varid < t->meta->ncol) {						
    col = t->p_col_ar[varid][gid];					
    vt = t->meta->type_ar[varid];					
  } else {								
    col = t->var_col_ar[varid][gid];					
    vt = t->var_type_ar[varid];						
  }
  if(start < 0) {
    s = NAN;
  } else {
    switch(vt) {
    case TSTYPE_INT:
      if(st->lastidx == 0) {
	for(i=start;i<=c;i++) {
	  s += ((int*)col)[i];
	}
	st->lastidx = c;
	st->lastold = ((int*)col)[start];
	st->lastnew = ((int*)col)[c];
	st->lastsum = s;
      } else if(st->lastidx == c) {
	s = st->lastsum - st->lastnew + ((int*)col)[c];
	st->lastnew = ((int*)col)[c];
	st->lastsum = s;
      } else {
	st->lastnew = ((int*)col)[c];
	s = st->lastsum - st->lastold + st->lastnew;
	st->lastold = ((int*)col)[start];
	st->lastidx = c;
	st->lastsum = s;
      }
      break;
    case TSTYPE_FLOAT:
      if(st->lastidx == 0 || isnan(st->lastsum)) {
	for(i=start;i<=c;i++) {
	  s += ((float*)col)[i];
	}
	st->lastidx = c;
	st->lastold = ((float*)col)[start];
	st->lastnew = ((float*)col)[c];
	st->lastsum = s;
      } else if(st->lastidx == c) {
	s = st->lastsum - st->lastnew + ((float*)col)[c];
	st->lastnew = ((float*)col)[c];
	st->lastsum = s;
      } else {
	st->lastnew = ((float*)col)[c];
	s = st->lastsum - st->lastold + st->lastnew;
	st->lastold = ((float*)col)[start];
	st->lastidx = c;
	st->lastsum = s;
      }
      break;
    case TSTYPE_DOUBLE:
      if(st->lastidx == 0 || isnan(st->lastsum)) {
	for(i=start;i<=c;i++) {
	  s += ((double*)col)[i];
	}
	st->lastidx = c;
	st->lastold = ((double*)col)[start];
	st->lastnew = ((double*)col)[c];
	st->lastsum = s;
      } else if(st->lastidx == c) {
	s = st->lastsum - st->lastnew + ((double*)col)[c];
	st->lastnew = ((double*)col)[c];
	st->lastsum = s;
      } else {
	st->lastnew = ((double*)col)[c];
	s = st->lastsum - st->lastold + st->lastnew;
	st->lastold = ((double*)col)[start];
	st->lastidx = c;
	st->lastsum = s;
      }
      break;
    default:
      (void)ts_warn(stderr, "runtime error @[%d|%d]: sum return type error.\n", vm->l, vm->c);	
      goto fail;
    }
    s /= length;
  }
  switch(rvt) {
  case TSTYPE_INT:
    *(int*)v = s;
  case TSTYPE_FLOAT:
    *(float*)v = s;
    break;
  case TSTYPE_DOUBLE:
    *(double*)v = s;
    break;
  default:
    (void)ts_warn(stderr, "runtime error @[%d|%d]: sum return type error.\n", vm->l, vm->c);	
    goto fail;
  }
  return 1;								
 fail:									
  return 0;								
}

int ts_table_ema(ts_table_t *t, short varid, int gid, int length, ts_ema_store_t *st, ts_type_t rvt, void *v)
{									
  TS_TRY(t && varid >=0 && gid >=0 && length > 0 && v);		
  void *col;
  ts_type_t vt;
  double e=0.0;
  int c = t->cur_row_ar[gid];
  
  if(varid < t->meta->ncol) {					
    col = t->p_col_ar[varid][gid];					
    vt = t->meta->type_ar[varid];					
  } else {								
    col = t->var_col_ar[varid][gid];				
    vt = t->var_type_ar[varid];					
  }
  switch(vt) {
  case TSTYPE_INT:
    if(st->lastidx == -1) {
      e = ((int*)col)[c];
      st->a = 2.0 / (length + 1);
      st->b = 1 - st->a;
      st->lastema = e;
      st->lastlastema = 0;
      st->lastidx = c;
    } else if(st->lastidx == c) {
      if(st->lastlastema < DBL_EPSILON) {
	e = ((int*)col)[c];
      } else {
	e = st->lastlastema * st->b + ((int*)col)[c] * st->a;
      }
      st->lastema = e;
    } else {
      e = st->lastema * st->b + ((int*)col)[c] * st->a;
      st->lastidx = c;
      st->lastlastema = st->lastema;
      st->lastema = e;
    }
    break;
  case TSTYPE_FLOAT:
    if(st->lastidx == -1) {
      e = ((float*)col)[c];
      st->a = 2.0 / (length + 1);
      st->b = 1 - st->a;
      st->lastema = e;
      st->lastlastema = 0;
      st->lastidx = c;
    } else if(st->lastidx == c) {
      if(st->lastlastema < DBL_EPSILON) {
	e = ((float*)col)[c];
      } else {
	e = st->lastlastema * st->b + ((float*)col)[c] * st->a;
      }
      st->lastema = e;
    } else {
      e = st->lastema * st->b + ((float*)col)[c] * st->a;
      st->lastidx = c;
      st->lastlastema = st->lastema;
      st->lastema = e;
    }
    break;
  case TSTYPE_DOUBLE:
    if(st->lastidx == -1) {
      e = ((double*)col)[c];
      st->a = 2.0 / (length + 1);
      st->b = 1 - st->a;
      st->lastema = e;
      st->lastlastema = 0;
      st->lastidx = c;
    } else if(st->lastidx == c) {
      if(st->lastlastema < DBL_EPSILON) {
	e = ((double*)col)[c];
      } else {
	e = st->lastlastema * st->b + ((double*)col)[c] * st->a;
      }
      st->lastema = e;
    } else {
      e = st->lastema * st->b + ((double*)col)[c] * st->a;
      st->lastidx = c;
      st->lastlastema = st->lastema;
      st->lastema = e;
    }
    break;								
  default:								
    (void)ts_warn(stderr, "runtime error @[%d|%d]: ema var type.\n", vm->l, vm->c);
    goto fail;
  }
  switch(rvt) {
  case TSTYPE_INT:
    *(int*)v = e;
  case TSTYPE_FLOAT:
    *(float*)v = e;
    break;
  case TSTYPE_DOUBLE:
    *(double*)v = e;
    break;
  default:
    (void)ts_warn(stderr, "runtime error @[%d|%d]: sum return type error.\n", vm->l, vm->c);	
    goto fail;
  }
  return 1;								
 fail:									
  return 0;								
}

int ts_table_ama(ts_table_t *t, short varid, int gid, int ERlen, int Flen, int Slen, ts_ama_store_t *st, ts_type_t rvt, void *v)
{
  TS_TRY(t && varid >=0 && gid >=0 && v);
  void *col;
  ts_type_t vt;
  double r, nc, tc, er, f;
  int i, c = t->cur_row_ar[gid];
  if(varid < t->meta->ncol) {
    col = t->p_col_ar[varid][gid];
    vt = t->meta->type_ar[varid];
  } else {
    col = t->var_col_ar[varid][gid];
    vt = t->var_type_ar[varid];
  }
  if(c < ERlen ) {
    switch(vt) {
    case TSTYPE_INT:
      if(st->lastidx == -1) {
	st->fctr = 2.0/(Flen+1) - 2.0/(Slen+1);
      }
      if(c == 0) {
	r = ((int*)col)[c];
	st->lastama = r;
	st->lastidx = c;
      } else if(st->lastidx == c) {
	tc = 0.0;
	for(i=c-ERlen+1;i<=c;i++) {
	  if(i > 0) {
	    tc += fabs(((int*)col)[i] - ((int*)col)[i-1]);
	  } else {
	    tc += fabs(((int*)col)[1] - ((int*)col)[0]);
	  }
	}
	nc = fabs(((int*)col)[c] - ((int*)col)[0]);
	er = tc > 0 ? nc / tc : 0;
	f = (2.0/(Slen+1) + er*st->fctr);
	f *= f;
	r = st->lastlastama + f*(((int*)col)[c] - st->lastlastama);
	st->lastama = r;
	st->lasttc = tc;
      } else {
	tc = 0.0;
	for(i=c-ERlen+1;i<=c;i++) {
	  if(i > 0) {
	    tc += fabs(((int*)col)[i] - ((int*)col)[i-1]);
	  } else {
	    tc += fabs(((int*)col)[1] - ((int*)col)[0]);
	  }
	}
	nc = fabs(((int*)col)[c] - ((int*)col)[0]);
	er = tc > 0 ? nc / tc : 0;
	f = (2.0/(Slen+1) + er*st->fctr);
	f *= f;
	r = st->lastama + f*(((int*)col)[c] - st->lastama);
	st->lastlastama = st->lastama;
	st->lastama = r;
	st->lastidx = c;   
	st->lasttc = tc;
      }
      break;
    case TSTYPE_FLOAT:
      if(st->lastidx == -1) {
	st->fctr = 2.0/(Flen+1) - 2.0/(Slen+1);
      }
      if(c == 0) {
	r = ((float*)col)[c];
	st->lastama = r;
	st->lastidx = c;
      } else if(st->lastidx == c) {
	tc = 0.0;
	for(i=c-ERlen+1;i<=c;i++) {
	  if(i > 0) {
	    tc += fabs(((float*)col)[i] - ((float*)col)[i-1]);
	  } else {
	    tc += fabs(((float*)col)[1] - ((float*)col)[0]);
	  }
	}
	nc = fabs(((float*)col)[c] - ((float*)col)[0]);
	er = tc > 0 ? nc / tc : 0;
	f = (2.0/(Slen+1) + er*st->fctr);
	f *= f;
	r = st->lastlastama + f*(((float*)col)[c] - st->lastlastama);
	st->lastama = r;
     	st->lasttc = tc;
      } else {
	tc = 0.0;
	for(i=c-ERlen+1;i<=c;i++) {
	  if(i > 0) {
	    tc += fabs(((float*)col)[i] - ((float*)col)[i-1]);
	  } else {
	    tc += fabs(((float*)col)[1] - ((float*)col)[0]);
	  }
	}
	nc = fabs(((float*)col)[c] - ((float*)col)[0]);
	er = tc > 0 ? nc / tc : 0;
	f = (2.0/(Slen+1) + er*st->fctr);
	f *= f;
	r = st->lastama + f*(((float*)col)[c] - st->lastama);
	st->lastlastama = st->lastama;
	st->lastama = r;
	st->lastidx = c;   
	st->lasttc = tc;
      }
      break;
    case TSTYPE_DOUBLE:
      if(st->lastidx == -1) {
	st->fctr = 2.0/(Flen+1) - 2.0/(Slen+1);
      }
      if(c == 0) {
	r = ((double*)col)[c];
	st->lastama = r;
	st->lastidx = c;
      } else if(st->lastidx == c) {
	tc = 0.0;
	for(i=c-ERlen+1;i<=c;i++) {
	  if(i > 0) {
	    tc += fabs(((double*)col)[i] - ((double*)col)[i-1]);
	  } else {
	    tc += fabs(((double*)col)[1] - ((double*)col)[0]);
	  }
	}
	nc = fabs(((double*)col)[c] - ((double*)col)[0]);
	er = tc > 0 ? nc / tc : 0;
	f = (2.0/(Slen+1) + er*st->fctr);
	f *= f;
	r = st->lastlastama + f*(((double*)col)[c] - st->lastlastama);
	st->lastama = r;  
	st->lasttc = tc;
      } else {
	tc = 0.0;
	for(i=c-ERlen+1;i<=c;i++) {
	  if(i > 0) {
	    tc += fabs(((double*)col)[i] - ((double*)col)[i-1]);
	  } else {
	    tc += fabs(((double*)col)[1] - ((double*)col)[0]);
	  }
	}
	nc = fabs(((double*)col)[c] - ((double*)col)[0]);
	er = tc > 0 ? nc / tc : 0;
	f = (2.0/(Slen+1) + er*st->fctr);
	f *= f;
	r = st->lastama + f*(((double*)col)[c] - st->lastama);
	st->lastlastama = st->lastama;
	st->lastama = r;
	st->lastidx = c;
	st->lasttc = tc;  
      }
      break;
    default:
      (void)ts_warn(stderr, "runtime error @[%d|%d]: ama var type.\n", vm->l, vm->c);	
      goto fail;
    }
  } else if(c >= ERlen) {
    switch(vt) {
    case TSTYPE_INT:
      if(c == ERlen) {
	if(st->lastidx == c) {
	  tc = st->lasttc - st->lastnewchg + fabs(((int*)col)[c] - ((int*)col)[c-1]);
	  st->lastnewchg = fabs(((int*)col)[c] - ((int*)col)[c-1]);
	  st->lasttc = tc;
	  nc = fabs(((int*)col)[c] - ((int*)col)[c-ERlen]);
	  er = tc > 0 ? nc / tc : 0;
	  f = (2.0/(Slen+1) + er*st->fctr);
	  f *= f;
	  r = st->lastlastama + f*(((int*)col)[c] - st->lastlastama);
	  st->lastama = r;
	} else {
	  st->lastnewchg = fabs(((int*)col)[c] - ((int*)col)[c-1]);
	  st->lastoldchg = fabs(((int*)col)[c-ERlen+1] - ((int*)col)[c-ERlen]);
	  tc = st->lasttc - st->lastoldchg + st->lastnewchg;
	  st->lasttc = tc;
	  st->lastidx = c;
	  nc = fabs(((int*)col)[c] - ((int*)col)[c-ERlen]);
	  er = tc > 0 ? nc / tc : 0;
	  f = (2.0/(Slen+1) + er*st->fctr);
	  f *= f;
	  r = st->lastama + f*(((int*)col)[c] - st->lastama);
	  st->lastlastama = st->lastama;
	  st->lastama = r;
	}
      } else if(st->lastidx == c){
	tc = st->lasttc - st->lastnewchg + fabs(((int*)col)[c] - ((int*)col)[c-1]);
	st->lastnewchg = fabs(((int*)col)[c] - ((int*)col)[c-1]);
	st->lasttc = tc;
	nc = fabs(((int*)col)[c] - ((int*)col)[c-ERlen]);
	er = tc > 0 ? nc / tc : 0;
	f = (2.0/(Slen+1) + er*st->fctr);
	f *= f;
	r = st->lastlastama + f*(((int*)col)[c] - st->lastlastama);
	st->lastama = r;
      } else {
	st->lastnewchg = fabs(((int*)col)[c] - ((int*)col)[c-1]);
	tc = st->lasttc - st->lastoldchg + st->lastnewchg;
	st->lastoldchg = fabs(((int*)col)[c-ERlen+1] - ((int*)col)[c-ERlen]);
	st->lastidx = c;
	st->lasttc = tc;
	nc = fabs(((int*)col)[c] - ((int*)col)[c-ERlen]);
	er = tc > 0 ? nc / tc : 0;
	f = (2.0/(Slen+1) + er*st->fctr);
	f *= f;
	r = st->lastama + f*(((int*)col)[c] - st->lastama);
	st->lastlastama = st->lastama;
	st->lastama = r;
      }
      break;
    case TSTYPE_FLOAT:
      if(c == ERlen) {
	if(st->lastidx == c) {
	  tc = st->lasttc - st->lastnewchg + fabs(((float*)col)[c] - ((float*)col)[c-1]);
	  st->lastnewchg = fabs(((float*)col)[c] - ((float*)col)[c-1]);
	  st->lasttc = tc;
	  nc = fabs(((float*)col)[c] - ((float*)col)[c-ERlen]);
	  er = tc > 0 ? nc / tc : 0;
	  f = (2.0/(Slen+1) + er*st->fctr);
	  f *= f;
	  r = st->lastlastama + f*(((float*)col)[c] - st->lastlastama);
	  st->lastama = r;
	} else {
	  st->lastnewchg = fabs(((float*)col)[c] - ((float*)col)[c-1]);
	  st->lastoldchg = fabs(((float*)col)[c-ERlen+1] - ((float*)col)[c-ERlen]);
	  tc = st->lasttc - st->lastoldchg + st->lastnewchg;
	  st->lasttc = tc;
	  st->lastidx = c;
	  nc = fabs(((float*)col)[c] - ((float*)col)[c-ERlen]);
	  er = tc > 0 ? nc / tc : 0;
	  f = (2.0/(Slen+1) + er*st->fctr);
	  f *= f;
	  r = st->lastama + f*(((float*)col)[c] - st->lastama);
	  st->lastlastama = st->lastama;
	  st->lastama = r;
	}
      } else if(st->lastidx == c){
	tc = st->lasttc - st->lastnewchg + fabs(((float*)col)[c] - ((float*)col)[c-1]);
	st->lastnewchg = fabs(((float*)col)[c] - ((float*)col)[c-1]);
	st->lasttc = tc;
	nc = fabs(((float*)col)[c] - ((float*)col)[c-ERlen]);
	er = tc > 0 ? nc / tc : 0;
	f = (2.0/(Slen+1) + er*st->fctr);
	f *= f;
	r = st->lastlastama + f*(((float*)col)[c] - st->lastlastama);
	st->lastama = r;
      } else {
	st->lastnewchg = fabs(((float*)col)[c] - ((float*)col)[c-1]);
	tc = st->lasttc - st->lastoldchg + st->lastnewchg;
	st->lastoldchg = fabs(((float*)col)[c-ERlen+1] - ((float*)col)[c-ERlen]);
	st->lastidx = c;
	st->lasttc = tc;
	nc = fabs(((float*)col)[c] - ((float*)col)[c-ERlen]);
	er = tc > 0 ? nc / tc : 0;
	f = (2.0/(Slen+1) + er*st->fctr);
	f *= f;
	r = st->lastama + f*(((float*)col)[c] - st->lastama);
	st->lastlastama = st->lastama;
	st->lastama = r;
      }
      break;
    case TSTYPE_DOUBLE:
      if(c == ERlen) {
	if(st->lastidx == c) {
	  tc = st->lasttc - st->lastnewchg + fabs(((double*)col)[c] - ((double*)col)[c-1]);
	  st->lastnewchg = fabs(((double*)col)[c] - ((double*)col)[c-1]);
	  st->lasttc = tc;
	  nc = fabs(((double*)col)[c] - ((double*)col)[c-ERlen]);
	  er = tc > 0 ? nc / tc : 0;
	  f = (2.0/(Slen+1) + er*st->fctr);
	  f *= f;
	  r = st->lastlastama + f*(((double*)col)[c] - st->lastlastama);
	  st->lastama = r;
	} else {
	  st->lastnewchg = fabs(((double*)col)[c] - ((double*)col)[c-1]);
	  st->lastoldchg = fabs(((double*)col)[c-ERlen+1] - ((double*)col)[c-ERlen]);
	  tc = st->lasttc - st->lastoldchg + st->lastnewchg;
	  st->lasttc = tc;
	  st->lastidx = c;
	  nc = fabs(((double*)col)[c] - ((double*)col)[c-ERlen]);
	  er = tc > 0 ? nc / tc : 0;
	  f = (2.0/(Slen+1) + er*st->fctr);
	  f *= f;
	  r = st->lastama + f*(((double*)col)[c] - st->lastama);
	  st->lastlastama = st->lastama;
	  st->lastama = r;
	}
      } else if(st->lastidx == c){
	tc = st->lasttc - st->lastnewchg + fabs(((double*)col)[c] - ((double*)col)[c-1]);
	st->lastnewchg = fabs(((double*)col)[c] - ((double*)col)[c-1]);
	st->lasttc = tc;
	nc = fabs(((double*)col)[c] - ((double*)col)[c-ERlen]);
	er = tc > 0 ? nc / tc : 0;
	f = (2.0/(Slen+1) + er*st->fctr);
	f *= f;
	r = st->lastlastama + f*(((double*)col)[c] - st->lastlastama);
	st->lastama = r;
      } else {
	st->lastnewchg = fabs(((double*)col)[c] - ((double*)col)[c-1]);
	tc = st->lasttc - st->lastoldchg + st->lastnewchg;
	st->lastoldchg = fabs(((double*)col)[c-ERlen+1] - ((double*)col)[c-ERlen]);
	st->lastidx = c;
	st->lasttc = tc;
	nc = fabs(((double*)col)[c] - ((double*)col)[c-ERlen]);
	er = tc > 0 ? nc / tc : 0;
	f = (2.0/(Slen+1) + er*st->fctr);
	f *= f;
	r = st->lastama + f*(((double*)col)[c] - st->lastama);
	st->lastlastama = st->lastama;
	st->lastama = r;
      }
      break;
    default:								
      (void)ts_warn(stderr, "runtime error @[%d|%d]: ama var type.\n", vm->l, vm->c);	
      goto fail;
    }
  }

  switch(rvt) {
  case TSTYPE_INT:
    *(int*)v = r;
  case TSTYPE_FLOAT:
    *(float*)v = r;
    break;
  case TSTYPE_DOUBLE:
    *(double*)v = r;
    break;
  default:
    (void)ts_warn(stderr, "runtime error @[%d|%d]: ama return type error.\n", vm->l, vm->c);	
    goto fail;
  }
  return 1;								
 fail:									
  return 0;
}

int ts_table_hhv(ts_table_t *t, short varid, int gid, int length, ts_type_t vt, void *v)
{
  TS_TRY(t && varid >= 0 && gid >= 0 && v && length > 0);
  void *col;
  if(varid < t->meta->ncol) {
    col = t->p_col_ar[varid][gid];
  } else {
    col = t->var_col_ar[varid][gid];
  }
  int c = t->cur_row_ar[gid];
  int start = c+1 < length ? 0 : c-length+1;
  int i;
  int vi;
  float vf;
  double vlf;
  switch(vt) {
  case TSTYPE_INT:
    vi = INT_MIN;
    for(i=start;i<=c;i++) {
      if(((int*)col)[i] > vi)
	vi = ((int*)col)[i];
    }
    *(int*)v = vi;
    break;
  case TSTYPE_FLOAT:
    vf = FLT_MIN;
    for(i=start;i<=c;i++) {
      if(((float*)col)[i] > vf)
	vf = ((float*)col)[i];
    }
    *(float*)v = vf;
    break;
  case TSTYPE_DOUBLE:
    vlf = DBL_MIN;
    for(i=start;i<=c;i++) {
      if(((double*)col)[i] > vlf)
	vlf = ((double*)col)[i];
    }
    *(double*)v = vlf;
    break;
  default:
    (void)ts_warn(stderr, "runtime error @[%d|%d]: hhv var type error.\n", vm->l, vm->c);
    goto fail;	
  }
  return 1;								
 fail:									
  return 0;
}									

int ts_table_llv(ts_table_t *t, short varid, int gid, int length, ts_type_t vt, void *v)
{
  TS_TRY(t && varid >= 0 && gid >= 0 && v && length > 0);
  void *col;
  if(varid < t->meta->ncol) {
    col = t->p_col_ar[varid][gid];
  } else {
    col = t->var_col_ar[varid][gid];
  }
  int c = t->cur_row_ar[gid];
  int start = c+1 < length ? 0 : c-length+1;
  int i;
  int vi;
  float vf;
  double vlf;
  switch(vt) {
  case TSTYPE_INT:
    vi = INT_MAX;
    for(i=start;i<=c;i++) {
      if(((int*)col)[i] < vi)
	vi = ((int*)col)[i];
    }
    *(int*)v = vi;
    break;
  case TSTYPE_FLOAT:
    vf = FLT_MAX;
    for(i=start;i<=c;i++) {
      if(((float*)col)[i] < vf)
	vf = ((float*)col)[i];
    }
    *(float*)v = vf;
    break;
  case TSTYPE_DOUBLE:
    vlf = DBL_MAX;
    for(i=start;i<=c;i++) {
      if(((double*)col)[i] < vlf)
	vlf = ((double*)col)[i];
    }
    *(double*)v = vlf;
    break;
  default:
    (void)ts_warn(stderr, "runtime error @[%d|%d]: llv var type error.\n", vm->l, vm->c);
    goto fail;	
  }
  return 1;								
 fail:									
  return 0;
}
