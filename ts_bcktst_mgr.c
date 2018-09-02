#include "gnuplot_i.h"
#include "ts_bcktst_mgr.h"
#include "ts_vm.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <pthread.h>

extern ts_vm_t *vm;

static int _ts_bcktst_mgr_determine_order_price(ts_bcktst_mgr_t *btm, int direction, int offset, double *res);
static void* run_optimize_one(void *argv);

typedef struct ts_bcktst_store {
  int runid;
  ts_prgm_mgr_t *pm;
  ts_vm_t *lvm;
  int nvar;
  double *var_v_ar;
} ts_bcktst_store_t;

ts_bcktst_mgr_t *ts_bcktst_mgr_new(int ngrp, void *v, char mode, double init_cash)
{
  TS_TRY(ngrp > 0);
  ts_bcktst_mgr_t *btm = calloc(1, sizeof(ts_bcktst_mgr_t));
  ts_vm_t *vm = v;
  TS_TRY(btm);
  int i;
  btm->ngrp = ngrp;
  btm->mode = mode;
  TS_TRY(btm->mtrd_ar = calloc(ngrp, sizeof(int)));
  TS_TRY(btm->ntrd_ar = calloc(ngrp, sizeof(int)));
  TS_TRY(btm->trade_ar = calloc(ngrp, sizeof(ts_trade_t**)));
  for(i=0;i<ngrp;i++) {
    btm->mtrd_ar[i] = 1000;
    btm->ntrd_ar[i] = 0;
    TS_TRY(btm->trade_ar[i] = calloc(1000, sizeof(ts_trade_t*)));
  }
  
  TS_TRY(ts_table_new_var(vm->cur_table, "pos", TSTYPE_INT, 0, &btm->pos_vid));
  TS_TRY(ts_table_new_var(vm->cur_table, "trdid", TSTYPE_INT, 0, &btm->trdid_vid));
  TS_TRY(ts_var_mgr_new_var(vm->cur_table->var_mgr, "lastEntryPrice", &btm->lastEntryPrice_varid));
  TS_TRY(ts_var_mgr_new_var(vm->cur_table->var_mgr, "position", &btm->position_varid));
  btm->vm = v;

  if(mode == 'f') {
    TS_TRY(ts_vm_table_load(vm, ".contract_fctr", 0, &btm->contract_fctr_tid));
    TS_TRY(ts_vm_table_load(vm, ".margin_rate", 0, &btm->margin_rate_tid));
  } else if(mode == 'e') {
  }
  btm->cash = init_cash;
  btm->init_cash = init_cash;
  return btm;
 fail:
  return 0;
}

int ts_bcktst_mgr_free(ts_bcktst_mgr_t *btm)
{
  TS_TRY(btm);
  int i=0;
  for(i=0;i<btm->ngrp;i++) {
    ts_free(btm->trade_ar[i]);
  }
  ts_free(btm->trade_ar);
  ts_free(btm->ntrd_ar);
  ts_free(btm->mtrd_ar);
  free(btm);
  return 1;
 fail:
  return 0;
}

ts_bcktst_store_t *ts_bcktst_store_new(int runid, int nvar, double *var_v_ar, const char *formula_name)
{
  TS_TRY(runid >=0 && nvar > 0 && var_v_ar);
  ts_bcktst_store_t *st = calloc(1, sizeof(ts_bcktst_store_t));
  st->runid = runid;
  TS_TRY(st->pm = ts_prgm_mgr_new());
  TS_TRY(st->lvm = ts_vm_new(st->pm));
  TS_TRY(st->var_v_ar = calloc(nvar, sizeof(double)));
  int i;
  for(i=0;i<nvar;i++) {
    st->var_v_ar[i] = var_v_ar[i];
  }
  st->nvar = nvar;
  TS_TRY(ts_prgm_mgr_load_main(st->pm, formula_name, 0));

  return st;
 fail:
  return 0;
}

int ts_bcktst_store_free(ts_bcktst_store_t *st)
{
  TS_TRY(st);
  ts_free(st->var_v_ar);
  if(st->pm) {
    TS_TRY(ts_prgm_mgr_free(st->pm));
  }
  if(st->lvm) {
    TS_TRY(ts_vm_free(st->lvm));
  }
  ts_free(st);
  return 1;
 fail:
  return 0;
}

int ts_bcktst_mgr_reset(ts_bcktst_mgr_t *btm)
{
  TS_TRY(btm);
  btm->cash = btm->init_cash;
  int i, j;
  for(i=0;i<btm->ngrp;i++) {
    for(j=0;j<btm->ntrd_ar[i];j++) {
      ts_free(btm->trade_ar[i][i]);
    }
    btm->ntrd_ar[i] = 0;
  }  
  return 1;
 fail:
  return 0;
}

int ts_trade_print(ts_trade_t *trd)
{
  TS_TRY(trd);
  printf("%d\t%s\t%.3lf\t%d\t%d\n", trd->trdid, ts_time_to_str(trd->time), trd->prc, trd->qty, trd->op);
  return 1;
 fail:
  return 0;
}

int ts_bcktst_mgr_store_trd(ts_bcktst_mgr_t *btm, int gid, double time, double prc, int qty, int op)
{
  TS_TRY(btm && gid >= 0 && (op == -1 || op == 1));
  ts_trade_t *t;
  TS_TRY(t = calloc(1, sizeof(ts_trade_t)));
  t->time = time;
  t->prc = prc;
  t->qty = qty;
  t->op = op;
  t->trdid = btm->ntrd_ar[gid];
  if(btm->ntrd_ar[gid] == btm->mtrd_ar[gid]) {
    ts_realloc(btm->trade_ar[gid], btm->mtrd_ar[gid], btm->mtrd_ar[gid], ts_trade_t*);
    btm->mtrd_ar[gid] += btm->mtrd_ar[gid];
  }
  btm->trade_ar[gid][btm->ntrd_ar[gid]] = t;
  btm->ntrd_ar[gid]++;
  return 1;
 fail:
  return 0;
}
int ts_bcktst_mgr_send_order(ts_bcktst_mgr_t *btm, int qty, int direction, int op, double price)
{
  if(!btm) {
    (void)ts_warn(stderr, "simulation module not loaded!");
    goto fail;
  }
  TS_TRY(btm->vm && (direction == 1 || direction == -1) && (op==1 || op==-1) && btm->pos_vid >= 0);
  ts_vm_t *vm = btm->vm;
  // OP    direction      op
  // buy          1        1
  // sell        -1       -1
  // short       -1        1
  // cover        1       -1
  TS_TRY(vm->cur_table->ngrp == btm->ngrp);

  double pos, delta = qty*direction;
  double ctime, prc;
  TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, btm->position_varid, vm->cur_gid, &pos));
  if(price < DBL_EPSILON) {
    TS_TRY(_ts_bcktst_mgr_determine_order_price(btm, direction, 0, &prc));
  } else {
    prc = price;
  }

  switch(direction) {
  case 1:
    switch(op) {
    case 1: // buy
      TS_TRY(ts_var_mgr_set_var(vm->cur_table->var_mgr, vm->btm->lastEntryPrice_varid, vm->cur_gid, prc));
      break;
    case -1: // cover
      if(pos >= 0) {
	return 1;
      }
      break;
    default:
      (void)ts_warn(stderr, "runtime error @[%d|%d]: SendOrder argv error.\n", vm->l, vm->c);
      goto fail;
    }
    break;
  case -1:
    switch(op) {
    case 1: // short
      TS_TRY(ts_var_mgr_set_var(vm->cur_table->var_mgr, vm->btm->lastEntryPrice_varid, vm->cur_gid, prc));
      break;
    case -1: // sell
      if(pos <= 0) {
	return 1;
      }
      break;
    default:
      (void)ts_warn(stderr, "runtime error @[%d|%d]: SendOrder argv error.\n", vm->l, vm->c);
      goto fail;
    }
    break;
  default:
    (void)ts_warn(stderr, "runtime error @[%d|%d]: SendOrder argv error.\n", vm->l, vm->c);
    goto fail;
  }

  TS_TRY(ts_table_set_vari(vm->cur_table, btm->pos_vid, vm->cur_gid, (int)(pos+delta)));
  TS_TRY(ts_var_mgr_set_var(vm->cur_table->var_mgr, btm->position_varid, vm->cur_gid, pos+delta));
  TS_TRY(ts_table_get_varlf(vm->cur_table, vm->cur_table->tcol, vm->cur_gid, &ctime));

  TS_TRY(ts_bcktst_mgr_store_trd(btm, vm->cur_gid, ctime, prc, qty*direction, op));
  TS_TRY(ts_table_set_vari(vm->cur_table, btm->trdid_vid, vm->cur_gid, btm->ntrd_ar[vm->cur_gid]));

  return 1;
 fail:
  return 0;
}


int _ts_bcktst_mgr_determine_order_price(ts_bcktst_mgr_t *btm, int direction, int offset, double *res)
{
  TS_TRY(btm && res && (direction == 1 || direction == -1));
  int prci;
  float prcf;
  double prc;
  ts_vm_t *vm = btm->vm;
  if(direction > 0) {
    switch(btm->bask_vt) {
    case TSTYPE_INT:
      TS_TRY(ts_table_get_vari_ref(vm->cur_table, btm->bask_col, vm->cur_gid, offset, &prci));
      prc = prci;
      break;
    case TSTYPE_FLOAT:
      TS_TRY(ts_table_get_varf_ref(vm->cur_table, btm->bask_col, vm->cur_gid, offset, &prcf));
      prc = prcf;
      break;
    case TSTYPE_DOUBLE:
      TS_TRY(ts_table_get_varlf_ref(vm->cur_table, btm->bask_col, vm->cur_gid, offset, &prc));
      break;
    default:
      (void)ts_warn(stderr, "runtime error @[%d|%d]: bestask price type error.", vm->l, vm->c);
      goto fail;
    }
  } else {
    switch(btm->bbid_vt) {
    case TSTYPE_INT:
      TS_TRY(ts_table_get_vari_ref(vm->cur_table, btm->bbid_col, vm->cur_gid, offset, &prci));
      prc = prci;
      break;
    case TSTYPE_FLOAT:
      TS_TRY(ts_table_get_varf_ref(vm->cur_table, btm->bbid_col, vm->cur_gid, offset, &prcf));
      prc = prcf;
      break;
    case TSTYPE_DOUBLE:
      TS_TRY(ts_table_get_varlf_ref(vm->cur_table, btm->bbid_col, vm->cur_gid, offset, &prc));
      break;
    default:
      (void)ts_warn(stderr, "runtime error @[%d|%d]: bestask price type error.", vm->l, vm->c);
      goto fail;
    }
  }
  *res = prc;
  return 1;
 fail:
  return 0;
}

int ts_bcktst_mgr_perf(ts_bcktst_mgr_t *btm, int run_gid, int display)
{
  TS_TRY(btm);
  
  int i, j;
  int n = btm->ntrd_ar[run_gid];
  int m = n/2;
  double *x_ar = alloca(m * sizeof(double));
  double *y_ar = alloca(m * sizeof(double));
  double *ret_ar = alloca(m * sizeof(double));
  double *daily_ret_ar = alloca(m * sizeof(double));
  int nday = 0;
  int cur_day = 0, day;
  double prev_day_eqt = btm->init_cash;
  double prev_high = 0;
  double cur_drawdown = 0;
  
  if(run_gid == -1) {
  } else {
    int direction;
    ts_trade_t *lasttrd = 0;
    ts_trade_t *trd = 0;
    double pos, prc;
    ts_vm_t *vm = btm->vm;
    if(display)
      printf("=============================================================================\n");
    for(i=0;i<n;i++) {
      trd = btm->trade_ar[run_gid][i];
      if(display)
	printf("%d\t%s\t%.6lf\t%d\t%d", trd->trdid, ts_time_to_str(trd->time), trd->prc, trd->qty, trd->op);
      btm->cash -= trd->prc * trd->qty;
      if(-1 == trd->op) {
	if(display)
	  printf("\tP&L: %.6lf\n", -(trd->prc*trd->qty+lasttrd->prc*lasttrd->qty));
	j = i/2;
	x_ar[j] = j;
	y_ar[j] = btm->cash - btm->init_cash;
	if(y_ar[j] > prev_high) {
	  prev_high = y_ar[j];
	} else {
	  cur_drawdown = prev_high - y_ar[j];
	  if(cur_drawdown > btm->max_drawdown) {
	    btm->max_drawdown = cur_drawdown;
	  }
	}
	ret_ar[j] = y_ar[j] - y_ar[j-1];
	day = ts_time_get_day(trd->time);
	if(day > cur_day) {
	  daily_ret_ar[nday] = btm->cash - prev_day_eqt;
	  nday++;
	  cur_day = day;
	  prev_day_eqt = btm->cash;
	}
	if(display)
	  printf("---------------------------------------------------------------\n");
      } else {
	lasttrd = trd;
	if(display)
	  printf("\n");
      }
    }
    TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, btm->position_varid, run_gid, &pos));
    if(fabs(pos) > DBL_EPSILON) {
      direction = pos > 0 ? 1 : -1;
      TS_TRY(_ts_bcktst_mgr_determine_order_price(btm, direction, 1, &prc));
      btm->cash += prc * trd->qty;
      if(display) {
	printf("Open order P&L: %.6lf\n", prc*trd->qty-trd->prc*trd->qty);
	printf("---------------------------------------------------------------\n");
      }
    }
    if(display) {
      printf("Total P&L: %.6lf\n", btm->cash - btm->init_cash);
      printf("=============================================================================\n");
    }
    btm->ret = btm->cash - btm->init_cash;
    daily_ret_ar[nday] = btm->cash - prev_day_eqt;
    nday++;
  }

  double mean = btm->ret / m;
  double s=0.0, std=0.0;
  //printf("ret_ar: \n");
  for(i=0;i<m;i++) {
    //printf("%.3lf\n", ret_ar[i]);
    s += pow((ret_ar[i] - mean), 2);
  }
  s /= m;
  std = sqrt(s);
  btm->sharp_ratio = btm->ret / std;
  //printf("sharp ratio: %.3lf\n", btm->ret / s);
  //printf("daily_ret_ar: \n");
  s = 0.0;
  mean = btm->ret / nday;
  for(i=0;i<nday;i++) {
    //printf("%.3lf\n", daily_ret_ar[i]);
    s += pow((daily_ret_ar[i] - mean), 2);
  }
  s /= nday;
  std = sqrt(s);
  btm->daily_sharp_ratio = btm->ret / std;
  //printf("daily sharp ratio: %.3lf\n", btm->ret / s);
  if(display) {
    gnuplot_ctrl *h = gnuplot_init();
    gnuplot_setstyle(h, "linespoints");
    gnuplot_plot_xy(h, x_ar, y_ar, m, "");
    //while(1)
    //  sleep(1);
    gnuplot_close(h);
  }
  return 1;
 fail:
  return 0;
}

int ts_func_buy(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argc >= 1 && argv && rv);

  TS_TRY(argv[0].type == TSTYPE_INT);
  if(argc == 1) {
    TS_TRY(ts_bcktst_mgr_send_order(vm->btm, argv[0].i, 1, 1, 0));
  } else {
    double prc;
    TS_TRY(ts_vm_get_var_valuelf(vm, vm->cur_table->var_mgr, &argv[1], &prc));
    TS_TRY(ts_bcktst_mgr_send_order(vm->btm, argv[0].i, 1, 1, prc));
  }

  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_sell(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argc >= 1 && argv && rv);

  TS_TRY(argv[0].type == TSTYPE_INT);
  if(argc == 1) {
    TS_TRY(ts_bcktst_mgr_send_order(vm->btm, argv[0].i, -1, -1, 0));
  } else {
    double prc;
    TS_TRY(ts_vm_get_var_valuelf(vm, vm->cur_table->var_mgr, &argv[1], &prc));
    TS_TRY(ts_bcktst_mgr_send_order(vm->btm, argv[0].i, -1, -1, prc));
  }
  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_short(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argc >= 1 && argv && rv);

  TS_TRY(argv[0].type == TSTYPE_INT);
  if(argc == 1) {
    TS_TRY(ts_bcktst_mgr_send_order(vm->btm, argv[0].i, -1, 1, 0));
  } else {
    double prc;
    TS_TRY(ts_vm_get_var_valuelf(vm, vm->cur_table->var_mgr, &argv[1], &prc));
    TS_TRY(ts_bcktst_mgr_send_order(vm->btm, argv[0].i, -1, 1, prc));
  }

  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_cover(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argc >= 1 && argv && rv);

  TS_TRY(argv[0].type == TSTYPE_INT);
  if(argc == 1) {
    TS_TRY(ts_bcktst_mgr_send_order(vm->btm, argv[0].i, 1, -1, 0));
  } else {
    double prc;
    TS_TRY(ts_vm_get_var_valuelf(vm, vm->cur_table->var_mgr, &argv[1], &prc));
    TS_TRY(ts_bcktst_mgr_send_order(vm->btm, argv[0].i, 1, -1, prc));
  }

  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

int ts_func_perf(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argc == 0 && argv && rv);
  TS_TRY(ts_bcktst_mgr_perf(vm->btm, vm->run_gid, 1));
  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;  
}

inline void _ts_bcktst_mgr_inc_var(int nvar, double *var_v_ar, double *start_ar, double *end_ar, double *stepsize_ar)
{
  int i = 0;
  if(stepsize_ar[i] > DBL_EPSILON)
    var_v_ar[i] += stepsize_ar[i];
  else
    var_v_ar[i] += 1;
  while(var_v_ar[i] - end_ar[i] > 1e-6) {
    var_v_ar[i] = start_ar[i];
    i++;
    if(i>=nvar)
      return;
    var_v_ar[i] += stepsize_ar[i];
  }
}

int ts_func_optimize(void *v, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv)
{
  ts_vm_t *vm = v;
  TS_TRY(vm && argc >=4 && argv && rv);
  TS_TRY(argv[0].type == TSTYPE_STR && argv[1].type == TSTYPE_STR && argv[2].type == TSTYPE_STR);
  char formula_name[BUFSIZ];
  snprintf(formula_name, BUFSIZ, "%s/%s.t", vm->prgm_mgr->basedir, argv[0].s);
  ts_bcktst_mgr_optm_method_t optm_method;
  int nrun;

  char *method = argv[1].s;
  if(0 == strcmp("max", method)) {
    optm_method = TS_BCKTST_MGR_OPTM_MAX;
  } else if(0 == strcmp("min", method)) {
    optm_method = TS_BCKTST_MGR_OPTM_MIN;
  } else {
    (void)ts_warn(stderr, "runtime error @[%d|%d]: no optimize method supplied.", vm->l, vm->c);
    goto fail;    
  }
  char *object = argv[2].s;
  if(0 == strcmp("return", object)) {
    if(0 != (argc-3) % 4) {
      (void)ts_warn(stderr, "runtime error @[%d|%d]: optimize argv error.", vm->l, vm->c);
      goto fail;
    }
    int i, nvar = (argc-3) / 4;
    int *steps_ar = alloca(nvar*sizeof(int));
    double *start_ar = alloca(nvar*sizeof(double));
    double *end_ar = alloca(nvar*sizeof(double));
    double *stepsize_ar = alloca(nvar*sizeof(double));
    double *var_v_ar = alloca(nvar*sizeof(double));
    char **varname_ar = alloca(nvar*sizeof(char*));
    TS_TRY(steps_ar && start_ar && end_ar && stepsize_ar && var_v_ar && varname_ar);
    memset(steps_ar, 0, nvar*sizeof(int));
    memset(start_ar, 0, nvar*sizeof(double));
    memset(end_ar, 0, nvar*sizeof(double));
    memset(stepsize_ar, 0, nvar*sizeof(double));
    memset(var_v_ar, 0, nvar*sizeof(double));

    nrun = 1;

    for(i=0;i<nvar;i++) {
      varname_ar[i] = argv[i*4+3].s;
      TS_TRY(ts_vm_get_var_valuelf(vm, 0, &argv[i*4+4], &start_ar[i]));
      TS_TRY(ts_vm_get_var_valuelf(vm, 0, &argv[i*4+5], &end_ar[i]));
      TS_TRY(ts_vm_get_var_valuelf(vm, 0, &argv[i*4+6], &stepsize_ar[i]));
      if(stepsize_ar[i] > DBL_EPSILON && end_ar[i] != start_ar[i]) {
	steps_ar[i] = ceil((end_ar[i]-start_ar[i])/stepsize_ar[i]) + 1;
      } else {
	steps_ar[i] = 1;
      }
      nrun *= steps_ar[i];
    }
    //////////////////////////////////
    int j=0;
    //sem_t *sem_ar = alloca(vm->threads * sizeof(sem_t));
    /*    for(i=0;i<vm->threads;i++) {
      TS_TRY(0 == sem_init(&sem_ar[i], 0, 0));
    }
    */
    ts_bcktst_store_t **st_ar = calloc(1, sizeof(ts_bcktst_store_t*));
    st_ar[0] = ts_bcktst_store_new(j, nvar, start_ar, formula_name);
    for(i=0;i<nvar;i++) {
      var_v_ar[i] = start_ar[i];
    }
    
    TS_TRY(ts_vm_run_once(st_ar[0]->lvm, st_ar[0]->pm->before));
    // check the parameter's consistency
    TS_TRY(nvar == st_ar[0]->pm->cur_formula->argc);
    for(i=0;i<nvar;i++) {
      TS_TRY(0 == strcmp(st_ar[0]->pm->cur_formula->argvname[i], varname_ar[i]));
    }
    // check if symbol specified
    if(-1 == st_ar[0]->lvm->run_gid) {
      (void)ts_warn(stderr, "runtime error @[%d|%d]: you should specify symbol before optimize run.", st_ar[0]->lvm->l, st_ar[0]->lvm->c);
      goto fail;
    }
    //int nthd = 0;
    //pthread_t tid;
    //pthread_attr_t attr;
    //TS_TRY(0 == pthread_attr_init(&attr));
    //TS_TRY(0 == pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
    while(j < nrun) {      
      /*if(nthd == vm->threads) {
	for(i=0;i<vm->threads;i++) {
	  TS_TRY(0 == sem_wait(&sem_ar[i]));
	}
	nthd = 0;
	}*/

      /*
      if(0 != j) {
	st_ar[j] = ts_bcktst_store_new(j, nvar, var_v_ar, formula_name);
	}
      */

      TS_TRY(run_optimize_one(st_ar[0]));
      //nthd++;

      j++;
      _ts_bcktst_mgr_inc_var(nvar, var_v_ar, start_ar, end_ar, stepsize_ar);
    }
    
    /*
    for(i=0;i<vm->threads;i++) {
      TS_TRY(0 == sem_wait(&sem_ar[i]));
    }
    */
  } else {
    (void)ts_warn(stderr, "runtime error @[%d|%d]: no optimize object specified.", vm->l, vm->c);
    goto fail;
  }  
  
  rv->i = 1;
  rv->type = TSTYPE_INT;
  return 1;
 fail:
  return 0;
}

void* run_optimize_one(void *argv)
{
  ts_bcktst_store_t *st = argv;
  TS_TRY(st);
  int i;

  //printf("%d\n", st->runid);
  if(0 != st->runid) {
    TS_TRY(ts_vm_run_once(st->lvm, st->pm->before));
  }
  for(i=0;i<st->nvar;i++) {
    TS_TRY(ts_var_mgr_new_var(st->lvm->cur_table->var_mgr, st->pm->cur_formula->argvname[i], &st->pm->cur_formula->svarid_ar[i]));
    TS_TRY(ts_var_mgr_set_var(st->lvm->cur_table->var_mgr, st->pm->cur_formula->svarid_ar[i], st->lvm->cur_gid, st->var_v_ar[i]));
  }
  
  TS_TRY(ts_vm_run(st->lvm, st->pm->cur_formula, VM_MODE_LIB));
  TS_TRY(ts_bcktst_mgr_perf(st->lvm->btm, st->lvm->run_gid, 0));
  for(i=0;i<st->nvar;i++) {
    printf("%.6lf\t", st->var_v_ar[i]);
  }
  printf("|\t%.6lf\t%.6lf\t%.6lf\t%.6lf\n", st->lvm->btm->ret, st->lvm->btm->sharp_ratio, st->lvm->btm->daily_sharp_ratio, st->lvm->btm->max_drawdown);

  //TS_TRY(0 == sem_post(st->sem));
  TS_TRY(ts_bcktst_store_free(st));


  return (void*)1;
 fail:
  return (void*)0;
}
