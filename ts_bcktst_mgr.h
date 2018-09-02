#ifndef TS_BCKTST_MGR_H
#define TS_BCKTST_MGR_H

#include "ts_type.h"
#include "ts_misc.h"
#include "ts_stack.h"

typedef struct ts_trade {
  int trdid;
  double prc;
  int qty;
  double time;
  int op; // Open Or Close
} ts_trade_t;

int ts_trade_print(ts_trade_t *trd);

typedef enum ts_bcktst_mgr_optm_method {
  TS_BCKTST_MGR_OPTM_MAX,
  TS_BCKTST_MGR_OPTM_MIN,
} ts_bcktst_mgr_optm_method_t;

typedef struct ts_bcktst_mgr {
  int ngrp;
  char mode;

  double init_cash;
  double cash;

  int *mtrd_ar;
  int *ntrd_ar;
  ts_trade_t ***trade_ar;
  short pos_vid;
  short trdid_vid;
  short bbid_col;
  ts_type_t bbid_vt;
  short bask_col;
  ts_type_t bask_vt;
  short lastEntryPrice_varid; // svar
  short position_varid; // svar

  // for future
  short contract_fctr_tid;
  short margin_rate_tid;

  double ret;
  double max_drawdown;
  double sharp_ratio;
  double daily_sharp_ratio;

  void *vm;
} ts_bcktst_mgr_t;

ts_bcktst_mgr_t *ts_bcktst_mgr_new(int ngrp, void *v, char mode, double init_cash);
int ts_bcktst_mgr_free(ts_bcktst_mgr_t *btm);
int ts_bcktst_mgr_reset(ts_bcktst_mgr_t *btm);
int ts_bcktst_mgr_store_trd(ts_bcktst_mgr_t *btm, int gid, double time, double prc, int qty, int op);
int ts_bcktst_mgr_perf(ts_bcktst_mgr_t *btm, int run_gid, int display);
int ts_bcktst_mgr_send_order(ts_bcktst_mgr_t *btm, int qty, int direction, int op, double price);

int ts_func_buy(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_sell(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_short(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_cover(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_perf(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);
int ts_func_optimize(void *vm, int argc, ts_stack_elem_t *argv, ts_stack_elem_t *rv);

#endif
