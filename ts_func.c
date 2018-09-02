#include "ts_func.h"
#include "ts_vm.h"
#include "ts_vm_macro.h"
#include <float.h>
#include <math.h>

int ts_func_abs(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argc == 1 && argv && rv);
  TS_TRY(ts_vm_get_var_valuelf(vm, vm->cur_table->var_mgr, &argv[0], &rv->lf));
  rv->lf = fabs(rv->lf);
  rv->type = TSTYPE_DOUBLE;
  return 1;
 fail:
  return 0;
}

int ts_func_max(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argc >= 2 && argv && rv);
  int i;
  double *v_ar = alloca(argc*sizeof(double));
  double max = 0.0;
  for(i=0;i<argc;i++) {
    TS_TRY(ts_vm_get_var_valuelf(vm, vm->cur_table->var_mgr, &argv[i], &v_ar[i]));
    if(v_ar[i] > max) 
      max = v_ar[i];
  }
  rv->lf = max;
  rv->type = TSTYPE_DOUBLE;
  return 1;
 fail:
  return 0;
}

int ts_func_min(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argc >= 2 && argv && rv);
  int i;
  double *v_ar = alloca(argc*sizeof(double));
  double min = DBL_MAX;
  for(i=0;i<argc;i++) {
    TS_TRY(ts_vm_get_var_valuelf(vm, vm->cur_table->var_mgr, &argv[i], &v_ar[i]));
    if(v_ar[i] < min) 
      min = v_ar[i];
  }
  rv->lf = min;
  rv->type = TSTYPE_DOUBLE;
  return 1;
 fail:
  return 0;
}

int ts_func_ref(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc==2 && rv);
  int varid;
  ts_type_t vt;
  int ref;
  //rv->type = argv[0].type;
  //TS_TRY(argv[1].type == TSTYPE_INT);
  TS_TRY(ts_vm_get_var_valuei(vm, vm->cur_table->var_mgr, &argv[1], &ref));
  switch(argv[0].type) {
  case TSTYPE_VARS:
    varid = ts_table_get_var_id(vm->cur_table, argv[0].s);
    break;
  case TSTYPE_VARI:
    varid = argv[0].vid;
    break;
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  vt = ts_table_get_var_type(vm->cur_table, varid);
  switch(vt) {
  case TSTYPE_INT:
    TS_TRY(ts_table_get_vari_ref(vm->cur_table, varid, vm->cur_gid, ref, &rv->i));
    rv->type = TSTYPE_INT;
    break;
  case TSTYPE_FLOAT:
    TS_TRY(ts_table_get_varf_ref(vm->cur_table, varid, vm->cur_gid, ref, &rv->f));
    rv->type = TSTYPE_FLOAT;
    break;
  case TSTYPE_DOUBLE:
    TS_TRY(ts_table_get_varlf_ref(vm->cur_table, argv[0].vid, vm->cur_gid, ref, &rv->lf));
    rv->type = TSTYPE_DOUBLE;
    break;
  case TSTYPE_TIME:
    TS_TRY(ts_table_get_varlf_ref(vm->cur_table, argv[0].vid, vm->cur_gid, ref, &rv->lf));
    rv->type = TSTYPE_TIME;
    break;
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  return 1;
 fail:
  return 0;
}

int ts_func_sum(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc==2 && rv);
  int varid;
  ts_type_t vt;
  int length;
  //TS_TRY(argv[1].type == TSTYPE_INT);
  TS_TRY(ts_vm_get_var_valuei(vm, vm->cur_table->var_mgr, &argv[1], &length));
  switch(argv[0].type) {
  case TSTYPE_VARS:
    varid = ts_table_get_var_id(vm->cur_table, argv[0].s);
    break;
  case TSTYPE_VARI:
    varid = argv[0].vid;
    break;
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  vt = ts_table_get_var_type(vm->cur_table, varid);
  rv->type = vt;
  int funcinstid = argv[argc].i;
  if(0 == vm->func_store_ar[funcinstid]) {
    //    if(vm->cur_table->ngrp > vm->ngrp) 
    vm->ngrp = vm->cur_table->ngrp;
    vm->func_store_ar[funcinstid] = calloc(vm->ngrp, sizeof(void*));
    int i;
    for(i=0;i<vm->ngrp;i++) {
      vm->func_store_ar[funcinstid][i] = calloc(2, sizeof(double));
    }
  }
  void *pst = vm->func_store_ar[funcinstid][vm->cur_gid];
  TS_TRY(ts_table_sum(vm->cur_table, argv[0].vid, vm->cur_gid, length, pst, vt, &rv->i));
  rv->type = vt;

  return 1;
 fail:
  return 0;
}


int ts_func_sma(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc == 2 && rv);
  int varid, length;
  ts_type_t vt;
  switch(argv[0].type) {
  case TSTYPE_VARS:
    varid = ts_table_get_var_id(vm->cur_table, argv[0].s);
    break;
  case TSTYPE_VARI:
    varid = argv[0].vid;
    break;
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  vt = ts_table_get_var_type(vm->cur_table, varid);
  rv->type = vt;
  TS_TRY(vt == TSTYPE_FLOAT || vt == TSTYPE_DOUBLE);
  TS_TRY(ts_vm_get_var_valuei(vm, vm->cur_table->var_mgr, &argv[1], &length));
  int funcinstid = argv[argc].i;
  if(0 == vm->func_store_ar[funcinstid]) {
    //if(vm->cur_table->ngrp > vm->ngrp) 
    vm->ngrp = vm->cur_table->ngrp;
    vm->func_store_ar[funcinstid] = calloc(vm->ngrp, sizeof(void*));
    int i;
    for(i=0;i<vm->ngrp;i++) {
      vm->func_store_ar[funcinstid][i] = calloc(1, sizeof(ts_sma_store_t));
    }
  }
  void *pst = vm->func_store_ar[funcinstid][vm->cur_gid];
  TS_TRY(ts_table_sma(vm->cur_table, varid, vm->cur_gid, length, pst, vt, &rv->f));

  return 1;
 fail:
  return 0;
}

int ts_func_ema(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc==2 && rv);
  int varid;
  ts_type_t vt;
  int length;
  switch(argv[0].type) {
  case TSTYPE_VARS:
    varid = ts_table_get_var_id(vm->cur_table, argv[0].s);
    break;
  case TSTYPE_VARI:
    varid = argv[0].vid;
    break;
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  vt = ts_table_get_var_type(vm->cur_table, varid);
  rv->type = vt;
  TS_TRY(vt == TSTYPE_FLOAT || vt == TSTYPE_DOUBLE);
  TS_TRY(ts_vm_get_var_valuei(vm, vm->cur_table->var_mgr, &argv[1], &length));
  int funcinstid = argv[argc].i;
  if(0 == vm->func_store_ar[funcinstid]) {
    //if(vm->cur_table->ngrp > vm->ngrp) 
    vm->ngrp = vm->cur_table->ngrp;
    vm->func_store_ar[funcinstid] = calloc(vm->ngrp, sizeof(void*));
    int i;
    for(i=0;i<vm->ngrp;i++) {
      ts_ema_store_t *pst = calloc(1, sizeof(ts_ema_store_t));
      pst->lastidx = -1;
      vm->func_store_ar[funcinstid][i] = pst;
    }
  }
  ts_ema_store_t *pst = vm->func_store_ar[funcinstid][vm->cur_gid];
  TS_TRY(ts_table_ema(vm->cur_table, varid, vm->cur_gid, length, pst, vt, &rv->f));

  return 1;
 fail:
  return 0;
}

int ts_func_ama(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc==4 && rv);
  int varid;
  ts_type_t vt;
  int l, f, s;
  switch(argv[0].type) {
  case TSTYPE_VARS:
    varid = ts_table_get_var_id(vm->cur_table, argv[0].s);
    break;
  case TSTYPE_VARI:
    varid = argv[0].vid;
    break;
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  vt = ts_table_get_var_type(vm->cur_table, varid);
  rv->type = vt;
  TS_TRY(vt == TSTYPE_FLOAT || vt == TSTYPE_DOUBLE);
  TS_TRY(ts_vm_get_var_valuei(vm, vm->cur_table->var_mgr, &argv[1], &l));
  TS_TRY(ts_vm_get_var_valuei(vm, vm->cur_table->var_mgr, &argv[2], &f));
  TS_TRY(ts_vm_get_var_valuei(vm, vm->cur_table->var_mgr, &argv[3], &s));
  int funcinstid = argv[argc].i;
  if(0 == vm->func_store_ar[funcinstid]) {
    //if(vm->cur_table->ngrp > vm->ngrp) 
    vm->ngrp = vm->cur_table->ngrp;
    vm->func_store_ar[funcinstid] = calloc(vm->ngrp, sizeof(void*));
    int i;
    for(i=0;i<vm->ngrp;i++) {
      ts_ama_store_t *pst = calloc(1, sizeof(ts_ama_store_t));
      pst->lastidx = -1;
      vm->func_store_ar[funcinstid][i] = pst;
    }
  }
  ts_ama_store_t *pst = vm->func_store_ar[funcinstid][vm->cur_gid];
  TS_TRY(ts_table_ama(vm->cur_table, varid, vm->cur_gid, l, f, s, pst, vt, &rv->f));

  return 1;
 fail:
  return 0;
}

int ts_func_hhv(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc == 2 && rv);
  int varid;
  int length;
  ts_type_t vt;
  switch(argv[0].type) {
  case TSTYPE_VARS:
    varid = ts_table_get_var_id(vm->cur_table, argv[0].s);
    break;
  case TSTYPE_VARI:
    varid = argv[0].vid;
    break;
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  vt = ts_table_get_var_type(vm->cur_table, varid);
  rv->type = vt;
  TS_TRY(ts_vm_get_var_valuei(vm, vm->cur_table->var_mgr, &argv[1], &length));
  TS_TRY(ts_table_hhv(vm->cur_table, varid, vm->cur_gid, length, vt, &rv->f));
  
  return 1;
 fail:
  return 0;
}

int ts_func_llv(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc == 2 && rv);
  int varid;
  int length;
  ts_type_t vt;
  switch(argv[0].type) {
  case TSTYPE_VARS:
    varid = ts_table_get_var_id(vm->cur_table, argv[0].s);
    break;
  case TSTYPE_VARI:
    varid = argv[0].vid;
    break;
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  vt = ts_table_get_var_type(vm->cur_table, varid);
  rv->type = vt;
  TS_TRY(ts_vm_get_var_valuei(vm, vm->cur_table->var_mgr, &argv[1], &length));
  TS_TRY(ts_table_llv(vm->cur_table, varid, vm->cur_gid, length, vt, &rv->f));
  
  return 1;
 fail:
  return 0;
}

int ts_func_table_create(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc >= 4 && rv); //at least have tablename, colname, colname1, type1
  int i;
  char *tablename = argv[0].s;
  int ncol = argv[1].i;
  TS_TRY(argc == ncol+5);
  char **colname_ar = (char**)alloca(ncol*sizeof(char*));
  ts_type_t *type_ar = (ts_type_t*)alloca(ncol*sizeof(ts_type_t));
  char *pb = 0, *gb = 0;
  ts_table_t *newtable;

  for(i=0;i<ncol;i++) {
    colname_ar[i] = argv[i+2].s;
  }
  TS_TRY(strlen(argv[ncol+2].s) == ncol);
  for(i=0;i<ncol;i++) {
    switch(argv[ncol+2].s[i]) {
    case 'i':
      type_ar[i] = TSTYPE_INT;
      break;
    case 'f':
      type_ar[i] = TSTYPE_FLOAT;
      break;
    case 'e':
      type_ar[i] = TSTYPE_DOUBLE;
      break;
    case 't':
      type_ar[i] = TSTYPE_TIME;
      break;
    case 's':
      type_ar[i] = TSTYPE_SYM;
      break;
    default:
      (void)ts_warn(stderr, "runtime error @[%d|%d]: wrong table type.\n", vm->l, vm->c);
      goto fail;
    }
  }
  pb = argv[ncol+3].s;
  gb = argv[ncol+4].s;
  
  newtable = ts_table_create(tablename, ncol,(const char**)colname_ar, type_ar, pb, gb);
  if(newtable) {
    newtable->meta->id = vm->ntable;
    TS_TRY(ts_hash_table_insert(vm->table_meta_table, tablename, newtable->meta));
    if(vm->ntable == vm->mtable) {
      TS_TRY(vm->table_ar = realloc(vm->table_ar, (vm->mtable+100)*sizeof(ts_table_t*)));
      memset(&vm->table_ar[vm->mtable], 0, 100*sizeof(ts_table_t*));
      vm->mtable += 100;
    }
    vm->table_ar[vm->ntable] = newtable;
    vm->cur_table = newtable;
    vm->ntable++;
  } else {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  rv->i = vm->ntable - 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_table_from_csv(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc >= 2 && rv); // at least have tablename, csvfilename
  int tid;
  char filename[BUFSIZ];
  if(argv[0].type == TSTYPE_INT) {
    tid = argv[0].i;
  } else if(argv[0].type == TSTYPE_STR){
    tid = ts_vm_get_table_id(vm, argv[0].s);
  } else {
    (void)ts_warn(stderr, "arg type error.\n");
    goto fail;
  }
  strcpy(filename, argv[1].s);
  ts_table_t *t = vm->table_ar[tid];
  int *filter_col_ar = 0;
  int i;
  if(argc > 2) {
    TS_TRY(argc-2 == t->meta->ncol);
    filter_col_ar = alloca(t->meta->ncol*sizeof(int));
    for(i=0;i<t->meta->ncol;i++) {
      filter_col_ar[i] = argv[i+2].i;
    }
  }
  char *s = strchr(filename, '*');
  char buf[BUFSIZ];
  if(s) {
    snprintf(buf, BUFSIZ, "ls -1 %s", filename);
    FILE *fp = popen(buf, "r");
    while((fgets(buf, BUFSIZ, fp))) {
      buf[strlen(buf)-1] = '\0';
      TS_TRY(ts_table_append_from_csv(t, buf, filter_col_ar, vm->time_format));
    }
  } else {
    TS_TRY(ts_table_append_from_csv(t, filename, filter_col_ar, vm->time_format));
  }

  vm->cur_table = t;

  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_table_write(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argc == 1 && argv && rv);
  int tid;
  if(argv[0].type == TSTYPE_INT) {
    tid = argv[0].i;
  } else if(argv[0].type == TSTYPE_STR){
    tid = ts_vm_get_table_id(vm, argv[0].s);
    if(-1 == tid) {
      (void)ts_warn(stderr, "runtime error @[%d|%d]: write table [%s] does not found.\n", vm->l, vm->c, argv[0].s);
      goto fail;
    }
  } else {
    (void)ts_warn(stderr, "arg type error.\n");
    goto fail;
  }
  ts_table_t *t = vm->table_ar[tid];
  TS_TRY(ts_table_write(t));
  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_table_load(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc == 1 && rv);
  char *tablename = argv[0].s;
  short tid;
  ts_table_t *t;
  TS_TRY(ts_vm_table_load(vm, tablename, &t, &tid));
  vm->cur_table = t;
  rv->i = tid;
  rv->type = TSTYPE_INT;

  return 1;
 fail:
  return 0;
}

int ts_func_table_append(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc>=2 && rv); // at least have tableid/name, values
  int tid, i;
  ts_type_t vt;
  short varid;
  if(argv[0].type == TSTYPE_INT) {
    tid = argv[0].i;
  } else if(argv[0].type == TSTYPE_STR){
    tid = ts_vm_get_table_id(vm, argv[0].s);
    if(-1 == tid) {
      (void)ts_warn(stderr, "runtime error @[%d|%d]: no table:%s found.", vm->l, vm->c, argv[0].s);
      goto fail;
    }
  } else {
    (void)ts_warn(stderr, "runtime error @[%d|%d]:arg type error.\n", vm->l, vm->c);
    goto fail;
  }
  ts_table_t *t = vm->table_ar[tid];
  void **data_ar;
  TS_TRY(argc == t->meta->ncol+1);
  data_ar = alloca(t->meta->ncol* sizeof(void*));
  for(i=0;i<t->meta->ncol;i++) {
    _ts_vm_pre_do_op1(&argv[i+1], vt);
    data_ar[i] = memdup(&argv[i+1].i, ts_type_size(vt));
  }
  TS_TRY(ts_table_append_row(t, (const void**)data_ar));
  for(i=0;i<t->meta->ncol;i++) {
    ts_free(data_ar[i]);
  }
  rv->i = 1;
  rv->type = TSTYPE_INT;  
  return 1;
 fail:
  return 0;
}

int ts_func_get_other(void *_v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = _v;
  TS_TRY(vm && argv && argc >= 3 && rv);
  short pid, tid, varid;
  char *varname;
  double v;
  TS_TRY(argv[0].type == TSTYPE_INT && argv[1].type == TSTYPE_INT);
  tid = argv[0].i;
  pid = argv[1].i;
  ts_prgm_t *prgm = vm->prgm_mgr->prgms_ar[pid];
  int nsvar = argc-3, i;
  if(nsvar != prgm->argc) {
    (void)ts_warn(stderr, "runtime error @[%d|%d]: formula argc does not match.\n", vm->l, vm->c);
    goto fail;
  }

  ts_table_t *pre_table = vm->cur_table;
  int gid = vm->cur_gid;
  ts_time_t ctime = ((ts_time_t*)pre_table->p_col_ar[pre_table->tcol][gid])[pre_table->cur_row_ar[gid]];
  ts_time_t end = vm->end;
  vm->end = ctime;
  vm->cur_table = vm->table_ar[tid];
  int pre_run_gid = vm->run_gid;
  vm->run_gid = ts_table_translate_gid(pre_table, vm->cur_table, gid); // same symbol but differnt gid
  int cur_row = pre_table->cur_row_ar[gid];
  
  double *svar_old = alloca(sizeof(double)*nsvar);
  for(i=0;i<nsvar;i++) {
    prgm->svarid_ar[i] = ts_var_mgr_get_var_id(vm->cur_table->var_mgr, prgm->argvname[i]);
    if(-1 == prgm->svarid_ar[i]) {
      svar_old[i] = 0;
      TS_TRY(ts_var_mgr_new_var(vm->cur_table->var_mgr, prgm->argvname[i], &prgm->svarid_ar[i]));
    } else {
      TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, prgm->svarid_ar[i], vm->run_gid, &svar_old[i]));
    }
    TS_TRY(ts_vm_get_var_valuelf(vm, pre_table->var_mgr, &argv[i+3], &v));
    TS_TRY(ts_var_mgr_set_var(vm->cur_table->var_mgr, prgm->svarid_ar[i], vm->run_gid, v));
  }
  
  //printf("\t%s:", prgm->name);
  TS_TRY(ts_vm_run(vm, prgm, VM_MODE_LIB));

  if(argv[2].type == TSTYPE_INT) {
    varid = argv[2].i;
  } else if(argv[2].type == TSTYPE_STR) {
    varname = argv[2].s;
    varid = ts_table_get_var_id(vm->table_ar[tid], varname);
    if(-1 == varid) {
      (void)ts_warn(stderr, "runtime error @[%d|%d]: var [%s] not found.\n", vm->l, vm->c, varname);
      goto fail;
    }
  }

  for(i=0;i<nsvar;i++) {
    TS_TRY(ts_var_mgr_set_var(vm->cur_table->var_mgr, prgm->svarid_ar[i], vm->run_gid, svar_old[i]));
  }
  vm->cur_table = pre_table;
  vm->cur_gid = gid;
  vm->end = end;

  TS_TRY(ts_vm_get_var(vm, tid, varid, rv));

  pre_table->cur_row_ar[gid] = cur_row;
  vm->run_gid = pre_run_gid;
  return 1;
 fail:
  return 0;
}

int ts_func_set_run_gid(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc == 1 && rv && vm->cur_table);

  unsigned int *ptr;
  if(argv[0].type == TSTYPE_STR) {
    if(0 == strcmp(argv[0].s, "all")) {
      vm->run_gid = -1;
    } else {
      ptr = (unsigned int*)ts_symbol_id_factory_get_id(vm->cur_table->idf, argv[0].s);
      if(!ptr) {
	(void)ts_warn(stderr, "runtime error @[%d|%d]: symbol %s does not in current table.\n", vm->l, vm->c, argv[0].s);
	goto fail;
      }
      vm->run_gid = *ptr;
    }
  } else {
    TS_TRY(argv[0].type == TSTYPE_INT);
    vm->run_gid = argv[0].i;
  }
  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_set_display(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc == 1 && rv && vm->cur_table);
  TS_TRY(argv[0].type == TSTYPE_STR);
  if(0 == strcmp(argv[0].s, "on")) {
    vm->display = 1;
  } else if(0 == strcmp(argv[0].s, "off")) {
    vm->display = 0;
  }
  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_load_module(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc >= 1 && rv);
  if(0 == strcmp(argv[0].s, "sim")) {
    TS_TRY(argc==5 && argv[1].type == TSTYPE_STR && argv[2].type == TSTYPE_VARI && argv[3].type == TSTYPE_VARI && argv[4].type == TSTYPE_INT);
    if(vm->cur_table->ngrp > 0 && 0 == vm->btm) {
      TS_TRY(vm->btm = ts_bcktst_mgr_new(vm->cur_table->ngrp, vm, argv[1].s[0], argv[4].i));
    }
  }

  vm->btm->bbid_col = argv[2].vid;
  vm->btm->bbid_vt = ts_table_get_var_type(vm->cur_table, argv[2].vid);
  vm->btm->bask_col = argv[3].vid;
  vm->btm->bask_vt = ts_table_get_var_type(vm->cur_table, argv[3].vid);

  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_set_time_format(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc == 1 && rv);
  if(argv[0].type != TSTYPE_STR) {
    (void)ts_warn(stderr, "set time format arg type error.\n");
    goto fail;
  }
  if(vm->time_format) {
    ts_free(vm->time_format);
  }
  vm->time_format = strdup(argv[0].s);
  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_set_threads(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc == 1 && rv);
  if(argv[0].type != TSTYPE_INT) {
    (void)ts_warn(stderr, "set thread arg type error.\n");
    goto fail;
  }
  vm->threads = argv[0].i;
  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_set_runrange(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc == 2 && rv);
  if(argv[0].type != TSTYPE_STR || argv[1].type != TSTYPE_STR) {
    (void)ts_warn(stderr, "set range arg type error.\n");
    goto fail;
  }
  vm->start = my_strptime(argv[0].s, vm->time_format);
  vm->end = my_strptime(argv[1].s, vm->time_format);

  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_print(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argv && argc > 0 && rv);
  int i;
  short varid;
  for(i=0;i<argc;i++) {
    switch(argv[i].type) {
    case TSTYPE_VARS:
      varid = ts_table_get_var_id(vm->cur_table, argv[i].s);
      if(-1 == varid) {
	varid = ts_var_mgr_get_var_id(vm->cur_table->var_mgr, argv[i].s);
	if(-1 == varid) {
	  (void)ts_warn(stderr, "runtime error @[%d|%d]:var [%s] not found.\n", vm->l, vm->c, argv[i].s);
	  goto fail;
	}
	TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, varid, vm->cur_gid, &argv[i].lf));
	argv[i].type = TSTYPE_DOUBLE;
	break;
      } else {
	argv[i].vid = varid;
	// continue to case TSTYPE_VARI;
      }
      break;
    case TSTYPE_VARI:
      argv[i].type = ts_table_get_var_type(vm->cur_table, argv[i].vid);
      switch(argv[i].type) {
      case TSTYPE_INT:
	TS_TRY(ts_table_get_vari(vm->cur_table, argv[i].vid, vm->cur_gid, &argv[i].i));
	break;
      case TSTYPE_FLOAT:
	TS_TRY(ts_table_get_varf(vm->cur_table, argv[i].vid, vm->cur_gid, &argv[i].f));
	break;
      case TSTYPE_DOUBLE:
      case TSTYPE_TIME:
	TS_TRY(ts_table_get_varlf(vm->cur_table, argv[i].vid, vm->cur_gid, &argv[i].lf));
	break;
      case TSTYPE_SYM:
	TS_TRY(ts_table_get_varsym(vm->cur_table, argv[i].vid, vm->cur_gid, &argv[i].s));
	break;
      default:
	(void)ts_warn(stderr, "\n");
	goto fail;
      }
      break;
    case TSTYPE_SVARI:
      TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, argv[i].vid, vm->cur_gid, &argv[i].lf));
      argv[i].type = TSTYPE_DOUBLE;
      break;
    default:
      break;
    }
    switch(argv[i].type) {
    case TSTYPE_INT:
      printf("%d ", argv[i].i);
      break;
    case TSTYPE_FLOAT:
      printf("%f ", argv[i].f);
      break;
    case TSTYPE_DOUBLE:
      printf("%lf ", argv[i].lf);
      break;
    case TSTYPE_STR:
      printf("%s ", argv[i].s);
      break;
    case TSTYPE_TIME:
      printf("%s ", ts_time_to_str(argv[i].lf));
      break;
    default:
      (void)ts_warn(stderr, "\n");
      goto fail;
    }
  }

  printf("\n");
  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}
