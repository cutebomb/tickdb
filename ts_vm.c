#include "ts_vm.h"
#include "ts_misc.h"
#include "ts_parse.h"
#include "ts_vm_macro.h"
#include <math.h>
#include <alloca.h>
#include <float.h>

static int _ts_vm_run(ts_vm_t *vm, ts_prgm_t *prgm);

ts_vm_t *ts_vm_new(ts_prgm_mgr_t *pm)
{
  ts_vm_t *vm = calloc(1, sizeof(ts_vm_t));
  TS_TRY(vm);
  TS_TRY(vm->stack = ts_stack_new());
  TS_TRY(vm->table_meta_table = ts_hash_table_new(1000, 0));

  vm->mtable = 100;
  vm->ntable = 0;
  TS_TRY(vm->table_ar = calloc(vm->mtable, sizeof(ts_table_t*)));
  vm->cur_table = 0;

  vm->time_format = strdup("%Y-%M-%d %h:%m:%s");

  int svar_ar1[] = {0}/*, svar_ar2[] = {1}*/;

  ts_func_meta_t ts_func_meta_ar[] =  {
    {0, "ref", 1, svar_ar1, ts_func_ref},
    {0, "sum", 1, svar_ar1, ts_func_sum},
    {0, "sma", 1, svar_ar1, ts_func_sma},
    {0, "ema", 1, svar_ar1, ts_func_ema},
    {0, "ama", 1, svar_ar1, ts_func_ama},
    {0, "hhv", 1, svar_ar1, ts_func_hhv},
    {0, "llv", 1, svar_ar1, ts_func_llv},
    {0, "max", 0, 0, ts_func_max},
    {0, "min", 0, 0, ts_func_min},
    {0, "abs", 0, 0, ts_func_abs},
    {0, "create", 0, 0, ts_func_table_create},
    {0, "appendcsv", 0, 0, ts_func_table_from_csv},
    {0, "write", 0, 0, ts_func_table_write},
    {0, "load", 0, 0, ts_func_table_load},
    {0, "append", 0, 0, ts_func_table_append},
    {0, "run", 0, 0, ts_func_set_run_gid},
    {0, ".getOther", 0, 0, ts_func_get_other},
    {0, "buy", 0, 0, ts_func_buy},
    {0, "sell", 0, 0, ts_func_sell},
    {0, "short", 0, 0, ts_func_short},
    {0, "cover", 0, 0, ts_func_cover},
    {0, "module", 0, 0, ts_func_load_module},
    {0, "timeformat", 0, 0, ts_func_set_time_format},
    {0, "perf", 0, 0, ts_func_perf},
    {0, "display", 0, 0, ts_func_set_display},
    {0, "optimize", 0, 0, ts_func_optimize},
    {0, "threads", 0, 0, ts_func_set_threads},
    {0, "run_range", 0, 0, ts_func_set_runrange},
    {0, "print", 0, 0, ts_func_print},
  };

  TS_TRY(vm->func_mgr = ts_func_mgr_new());
  int n = sizeof(ts_func_meta_ar) / sizeof(ts_func_meta_ar[0]), i;
  for(i=0;i<n;i++) {
    TS_TRY(ts_func_mgr_register_func(vm->func_mgr, ts_func_meta_ar[i].func_name, ts_func_meta_ar[i].nsvar, ts_func_meta_ar[i].sfunc_svar_idx_ar, ts_func_meta_ar[i].func));
  }
  
  vm->run_gid = -1;
  vm->display = 1;
  vm->threads = 1;
  vm->prgm_mgr = pm;
  vm->start = 0;
  vm->end = DBL_MAX;

  return vm;
 fail:
  return 0;
}

int ts_vm_free(ts_vm_t *vm)
{
  TS_TRY(vm);
  int i;
  for(i=0;i<vm->ntable;i++) {
    ts_table_free(vm->table_ar[i]);
  }
  if(vm->table_meta_table) {
    ts_hash_table_free(vm->table_meta_table);
  }
  if(vm->func_mgr) {
    ts_func_mgr_free(vm->func_mgr);
  }
  if(vm->stack) {
    ts_stack_free(vm->stack);
  }
  ts_free(vm);
  return 1;
 fail:
  return 0;
}

int ts_vm_run(ts_vm_t *vm, ts_prgm_t *pr, ts_vm_run_mode_t mode)
{
  TS_TRY(vm && pr && mode < VM_MODE_MAX);
  int i, p, j;
  ts_table_t *ct = vm->cur_table;
  short svarid;  
  ts_time_t ctime, start = vm->start, end = vm->end;

  if(pr->nins <= 0) {
    return 1;
  }
  if(!ct) {
    (void)ts_warn(stderr, "runtime error @[%d|%d]: cur_table not specified.\n", vm->l, vm->c);
    goto fail;
  }
  if(!vm->func_store_ar || pr->mfuncinstid > vm->nfunc) {
    vm->func_store_ar = realloc(vm->func_store_ar, sizeof(void**)*pr->mfuncinstid);
    memset(&vm->func_store_ar[vm->nfunc], 0, sizeof(void*)*(pr->mfuncinstid - vm->nfunc));
    vm->nfunc = pr->mfuncinstid;
  }

  if(mode == VM_MODE_MAIN) {
    //////////////////////////main///////////////////////////////
    for(i=0;i<pr->argc;i++) {
      svarid = ts_var_mgr_get_var_id(vm->cur_table->var_mgr, pr->argvname[i]);
      if(-1 == svarid) {
	TS_TRY(ts_var_mgr_new_var(vm->cur_table->var_mgr, pr->argvname[i], &svarid));
      }
      for(j=0;j<ct->ngrp;j++) {
	TS_TRY(ts_var_mgr_set_var(vm->cur_table->var_mgr, svarid, j, pr->argv[i]));
      }
    }

    if(ct->npar == 0) {
      if(vm->run_gid == -1) {
	for(i=0;i<ct->ngrp;i++) {
	  vm->cur_gid = i;
	  for(j=ct->cur_row_ar[i];j<ct->nrow_ar[i];j++,ct->cur_row_ar[i]=j) {
	    ctime = ((ts_time_t*)ct->p_col_ar[ct->tcol][i])[j];
	    if(ctime < start)
	      continue;
	    if(ctime > end)
	      break;
	    TS_TRY(ts_table_copy_previous_var_all(ct, i));
	    _ts_vm_run(vm, pr); // may be fail some how, but does not stop runing
	    TS_TRY(ts_stack_pop(vm->stack));
	  }
	  if(vm->display)
	    TS_TRY(ts_table_cur_var_print(ct, i, -1));
	}
      } else {
	vm->cur_gid = vm->run_gid;
	i = vm->run_gid;
	for(j=ct->cur_row_ar[i];j<ct->nrow_ar[i];j++,ct->cur_row_ar[i]=j) {
	  ctime = ((ts_time_t*)ct->p_col_ar[ct->tcol][i])[j];
	  if(ctime < start)
	    continue;
	  if(ctime > end)
	    break;
	  TS_TRY(ts_table_copy_previous_var_all(ct, i));
	  TS_TRY(_ts_vm_run(vm, pr));
	  if(vm->display)
	    TS_TRY(ts_table_cur_var_print(ct, i, 0));
	  TS_TRY(ts_stack_pop(vm->stack));
	}
      }
    } else {
      for(p=0;p<ct->npar;p++) {
	if(vm->run_gid == -1) {
	  for(i=0;i<ct->ngrp;i++) {
	    vm->cur_gid = i;
	    for(j=ct->cur_row_ar[i];j<ct->nrow_ar[i];j++,ct->cur_row_ar[i]=j) {
	      ctime = ((ts_time_t*)ct->p_col_ar[ct->tcol][i])[j];
	      if(ctime < start)
		continue;
	      if(ctime > end)
		break;
	      TS_TRY(ts_table_copy_previous_var_all(ct, i));
	      _ts_vm_run(vm, pr); // may be fail some how, but does not stop runing
	      TS_TRY(ts_stack_pop(vm->stack));
	    }
	    if(vm->display)
	      TS_TRY(ts_table_cur_var_print(ct, i, -1));
	  }
	} else {
	  vm->cur_gid = vm->run_gid;
	  i = vm->run_gid;
	  for(j=ct->cur_row_ar[i];j<ct->nrow_ar[i];j++,ct->cur_row_ar[i]=j) {
	    ctime = ((ts_time_t*)ct->p_col_ar[ct->tcol][i])[j];
	    if(ctime < start)
	      continue;
	    if(ctime > end)
	      break;
	    TS_TRY(ts_table_copy_previous_var_all(ct, i));
	    TS_TRY(_ts_vm_run(vm, pr));
	    if(vm->display)
	      TS_TRY(ts_table_cur_var_print(ct, i, 0));
	    TS_TRY(ts_stack_pop(vm->stack));
	  }
	}
	TS_TRY(ts_table_partition_next(ct));
      }
    }
    TS_TRY(ts_vm_clear(vm));
  } else if(mode == VM_MODE_LIB) {
    ///////////////////////////lib///////////////////////////
    int j;
    if(ct->npar == 0) {
      if(vm->run_gid == -1) {
	for(i=0;i<ct->ngrp;i++) {
	  vm->cur_gid = i;
	  for(j=ct->cur_row_ar[i];j<ct->nrow_ar[i];j++,ct->cur_row_ar[i]=j) {
	    ctime = ((ts_time_t*)ct->p_col_ar[ct->tcol][i])[j];
	    if(ctime < start)
	      continue;
	    if(ctime > end)
	      break;
	    TS_TRY(ts_table_copy_previous_var_part(ct, i, pr->nvar, pr->varid_ar));
	    _ts_vm_run(vm, pr); // may be fail some how, but does not stop runing
	    TS_TRY(ts_stack_pop(vm->stack));
	  }
	  //TS_TRY(ts_table_cur_var_print(ct, i, -1));
	}
      } else {
	vm->cur_gid = vm->run_gid;
	i = vm->run_gid;
	for(j=ct->cur_row_ar[i];j<ct->nrow_ar[i];j++,ct->cur_row_ar[i]=j) {
	  ctime = ((ts_time_t*)ct->p_col_ar[ct->tcol][i])[j];
	  if(ctime < start)
	    continue;
	  if(ctime > end)
	    break;
	  TS_TRY(ts_table_copy_previous_var_part(ct, i, pr->nvar, pr->varid_ar));
	  TS_TRY(_ts_vm_run(vm, pr));
	  //TS_TRY(ts_table_cur_var_print(ct, i, 0));
	  TS_TRY(ts_stack_pop(vm->stack));
	}
      }
    } else {
      for(p=0;p<ct->npar;p++) {
	if(vm->run_gid == -1) {
	  for(i=0;i<ct->ngrp;i++) {
	    vm->cur_gid = i;
	    for(j=ct->cur_row_ar[i];j<ct->nrow_ar[i];j++,ct->cur_row_ar[i]=j) {
	      ctime = ((ts_time_t*)ct->p_col_ar[ct->tcol][i])[j];
	      if(ctime < start)
		continue;
	      if(ctime > end)
		break;
	      TS_TRY(ts_table_copy_previous_var_part(ct, i, pr->nvar, pr->varid_ar));
	      _ts_vm_run(vm, pr); // may be fail some how, but does not stop runing
	      TS_TRY(ts_stack_pop(vm->stack));
	    }
	    //TS_TRY(ts_table_cur_var_print(ct, i, -1));
	  }
	} else {
	  vm->cur_gid = vm->run_gid;
	  i = vm->run_gid;
	  for(j=ct->cur_row_ar[i];j<ct->nrow_ar[i];j++,ct->cur_row_ar[i]=j) {
	    ctime = ((ts_time_t*)ct->p_col_ar[ct->tcol][i])[j];
	    if(ctime < start)
	      continue;
	    if(ctime > end)
	      break;
	    TS_TRY(ts_table_copy_previous_var_part(ct, i, pr->nvar, pr->varid_ar));
	    TS_TRY(_ts_vm_run(vm, pr));
	    //TS_TRY(ts_table_cur_var_print(ct, i, 0));
	    TS_TRY(ts_stack_pop(vm->stack));
	  }
	}
	TS_TRY(ts_table_partition_next(ct));
      }
    }
  } else if(mode == VM_MODE_SEQ) {
    int gid;
    if(ct->npar == 0) {
      for(i=0;i<ct->cur_nelem;i++) {
	gid = ct->cur_seq_ar[i];
	ctime = ((ts_time_t*)ct->p_col_ar[ct->tcol][gid])[i];
	if(ctime < start)
	  continue;
	if(ctime > end)
	  break;
	TS_TRY(ts_table_copy_previous_var_all(ct, gid));
	_ts_vm_run(vm, pr); // may be fail some how, but does not stop runing
	TS_TRY(ts_stack_pop(vm->stack));
      }
    } else {
      for(p=0;p<ct->npar;p++) {
	for(i=0;i<ct->cur_nelem;i++) {
	  gid = ct->cur_seq_ar[i];
	  ctime = ((ts_time_t*)ct->p_col_ar[ct->tcol][gid])[i];
	  if(ctime < start)
	    continue;
	  if(ctime > end)
	    break;
	  TS_TRY(ts_table_copy_previous_var_all(ct, gid));
	  _ts_vm_run(vm, pr); // may be fail some how, but does not stop runing
	  TS_TRY(ts_stack_pop(vm->stack));
	}
	TS_TRY(ts_table_partition_next(ct));
      }
    }
  }
  return 1;
 fail:
  return 0;
}

int ts_vm_run_once(ts_vm_t *vm, ts_prgm_t *pr)
{
  TS_TRY(vm && pr);
  if(pr->nins <= 0) {
    return 1;
  }
  TS_TRY(_ts_vm_run(vm, pr));
  TS_TRY(ts_stack_pop(vm->stack));
  return 1;
 fail:
  return 0;
}

int _ts_vm_run(ts_vm_t *vm, ts_prgm_t *p)
{
  TS_TRY(vm && p);
  int nins = p->nins;
  instruction_t *prgm = p->prgm_ar;
  int pc;
  float tf;
  short varid, funcid, subid, tid, pid;
  instruction_t *ins;
  ts_stack_elem_t vb, *a, *b=&vb, v;
  ts_type_t vt1, vt2, vt;
  int top = vm->stack->top, ti;
  memset(&v, 0, sizeof(ts_stack_elem_t));
  for(pc = 0; pc < nins; pc++) {
    ins = &prgm[pc];
    //printf("%d\t", pc);
    vm->l = ins->l;
    vm->c = ins->c;
    switch(ins->opcode) {
    case OP_NOP:
      break;
    case OP_PUSHG:
      switch(ins->type) {
      case TSTYPE_VARS:
	varid = ts_table_get_var_id(vm->cur_table, ins->s);
	if(-1 == varid) {
	  varid = ts_var_mgr_get_var_id(vm->cur_table->var_mgr, ins->s);
	  if(-1 == varid) {
	    //(void)ts_warn(stderr, "runtime waring @[%d|%d]: var [%s] not found, create it.\n", vm->l, vm->c, ins->s);
	    TS_TRY(ts_table_new_var(vm->cur_table, ins->s, TSTYPE_DOUBLE, 0, &varid));
	    v.type = TSTYPE_DOUBLE;
	    v.lf = 0;
	    ins->vid = varid;
	    ins->type = TSTYPE_VARI;
	    TS_TRY(ts_stack_push(vm->stack, &v));
	  } else {
	    TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, varid, vm->cur_gid, &v.lf));
	    v.type = TSTYPE_DOUBLE;
	    ins->vid = varid;
	    ins->type = TSTYPE_SVARI;
	    TS_TRY(ts_stack_push(vm->stack, &v));
	  }
	} else {
	  ins->vid = varid;
	  ins->type = TSTYPE_VARI;
	  TS_TRY(ts_stack_push(vm->stack, &ins->oprand));
	}
	ins->opcode = OP_PUSH;
	break;
      default:
	ins->opcode = OP_PUSH;
	TS_TRY(ts_stack_push(vm->stack, &ins->oprand));
	break;
      }
      break;
    case OP_PUSH:
      TS_TRY(ts_stack_push(vm->stack, &ins->oprand));
      break;
    case OP_POP:
      TS_TRY(ts_stack_pop(vm->stack));
      break;
    case OP_JMP:
      pc = ins->i-1;
      break;
    case OP_JMPF:
      TS_TRY(ts_stack_pop_to(vm->stack, b));
      if(!b->i) {
	pc = ins->i-1;
      }
      break;
    case OP_ADD:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      vt = max(vt1, vt2);
      _ts_vm_do_op2(a, a, b, vt1, vt2, +, vt);
      break;
    case OP_MINUS:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      vt = max(vt1, vt2);
      _ts_vm_do_op2(a, a, b, vt1, vt2, -, vt);
      break;
    case OP_MUL:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      vt = max(vt1, vt2);
      _ts_vm_do_op2(a, a, b, vt1, vt2, *, vt);
      break;
    case OP_DIV:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      vt = max(vt1, vt2);
      _ts_vm_do_op2(a, a, b, vt1, vt2, /, vt);
      break;
    case OP_MOD:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      vt = max(vt1, vt2);
      _ts_vm_do_func_op2(a, a, b, vt1, vt2, fmod, vt);
      break;
    case OP_NEG:
      TS_TRY(a = ts_stack_top(vm->stack));
      _ts_vm_pre_do_op1(a, vt1);
      _ts_vm_do_op1(a, a, vt1, -, vt1);
      break;
    case OP_NOT:
      TS_TRY(a = ts_stack_top(vm->stack));
      _ts_vm_pre_do_op1(a, vt1);
      _ts_vm_do_op1_logical(a, a, vt1, !);
      break;
    case OP_LT:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      _ts_vm_do_op2_logical(a, a, b, vt1, vt2, <);
      break;
    case OP_GT:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      _ts_vm_do_op2_logical(a, a, b, vt1, vt2, >);
      break;
    case OP_EQ:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      _ts_vm_do_op2_logical(a, a, b, vt1, vt2, ==);
      break;
    case OP_NEQ:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      _ts_vm_do_op2_logical(a, a, b, vt1, vt2, !=);
      break;
    case OP_LE:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      _ts_vm_do_op2_logical(a, a, b, vt1, vt2, <=);
      break;
    case OP_GE:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      _ts_vm_do_op2_logical(a, a, b, vt1, vt2, >=);
      break;
    case OP_AND:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      _ts_vm_do_op2_logical(a, a, b, vt1, vt2, &&);
      break;
    case OP_OR:
      _ts_vm_pre_do_op2(a, b, vt1, vt2);
      _ts_vm_do_op2_logical(a, a, b, vt1, vt2, ||);
      break;
    case OP_DOT:
      tid = ts_vm_get_table_id(vm, ins->tablename);
      if(-1 == tid) { // try do OP_DOTT
	varid = ts_table_get_var_id(vm->cur_table, ins->tablename);
	if(-1 == varid) {
	  (void)ts_warn(stderr, "runtime error @[%d|%d]: dot error.\n", vm->l, vm->c);
	  goto fail;
	}
	TS_TRY(ts_time_sub_to_subi(ins->varname, &subid));
	TS_TRY(ts_table_get_varlf(vm->cur_table, varid, vm->cur_gid, &v.lf));
	v.i = ts_time_get_subi(v.lf, subid);
	v.type = TSTYPE_INT;
	TS_TRY(ts_stack_push(vm->stack, &v));
	ins->tid = varid;
	ins->vid = subid;
	ins->opcode = OP_DOTT;
      } else {
	v.tid = tid;
	v.vid = ts_table_get_var_id(vm->table_ar[tid], ins->varname);
	ins->tid = tid;
	ins->vid = v.vid;
	TS_TRY(ts_vm_get_var(vm, tid, v.vid, &v));
	TS_TRY(ts_stack_push(vm->stack, &v));
	ins->opcode = OP_DOTI;
      }
      break;
    case OP_DOTI:
      TS_TRY(ts_vm_get_var(vm, ins->tid, ins->vid, &v));
      TS_TRY(ts_stack_push(vm->stack, &v));
      break;
    case OP_DOTT:
      TS_TRY(ts_table_get_varlf(vm->cur_table, ins->tid, vm->cur_gid, &v.lf));
      v.i = ts_time_get_subi(v.lf, ins->vid);
      v.type = TSTYPE_INT;
      TS_TRY(ts_stack_push(vm->stack, &v));
      break;
    case OP_CALL:
      TS_TRY(ts_stack_pop_to(vm->stack, b)); // argc
      ti = b->i;
      TS_TRY(ts_func_mgr_get_funcid(vm->func_mgr, ins->s, &funcid));
      if(0 == strcmp(".getOther", ins->s)) {
	ins[-ti-2].opcode = OP_PUSH;
	ins[-ti-2].type = TSTYPE_INT;
	tid = ts_vm_get_table_id(vm, ins[-ti-2].s);
	if(-1 == tid) {
	  (void)ts_warn(stderr, "runtime error @[%d|%d]: table [%s] not found.\n", vm->l, vm->c, ins[-ti-2].s);
	  goto fail;
	}
	ins[-ti-2].i = tid;
	vm->stack->stack_ar[vm->stack->top-ti-1].i = tid;
	vm->stack->stack_ar[vm->stack->top-ti-1].type = TSTYPE_INT;
	///////////////
	ins[-ti-1].opcode = OP_PUSH;
	ins[-ti-1].type =TSTYPE_INT;
	TS_TRY(ts_prgm_mgr_load_lib(vm->prgm_mgr, ins[-ti-1].s, &pid));
	ins[-ti-1].i = pid;
	vm->stack->stack_ar[vm->stack->top-ti].i = pid;
	vm->stack->stack_ar[vm->stack->top-ti].type = TSTYPE_INT;
	
	TS_TRY(ts_func_mgr_call(vm->func_mgr, vm, funcid, ti, &vm->stack->stack_ar[vm->stack->top-ti-1], &v));

	//////////////////
	ins[-ti].opcode = OP_PUSH;
	ins[-ti].type = TSTYPE_INT;
	varid = ts_table_get_var_id(vm->table_ar[tid], ins[-ti].s);
	if(-1 == varid) {
	  (void)ts_warn(stderr, "runtime error @[%d|%d]: var [%s] not found.\n", vm->l, vm->c, ins[-ti].s);
	  goto fail;
	}
	ins[-ti].i = varid;
      } else {
	TS_TRY(ts_func_mgr_call(vm->func_mgr, vm, funcid, ti, &vm->stack->stack_ar[vm->stack->top-ti-1], &v)); // last one is funcinstid
      }
      vm->stack->top -= (ti+1);
      TS_TRY(ts_stack_push(vm->stack, &v));
      ins->opcode = OP_CALLI;
      ins->fid = funcid;
      ins->type = TSTYPE_VARI;
      break;
    case OP_CALLI:
      TS_TRY(ts_stack_pop_to(vm->stack, b)); // argc
      TS_TRY(ts_func_mgr_call(vm->func_mgr, vm, ins->fid, b->i, &vm->stack->stack_ar[vm->stack->top-b->i-1], &v));
      vm->stack->top -= (b->i+1);
      TS_TRY(ts_stack_push(vm->stack, &v));
      break;
    case OP_SETSVAR:
      TS_TRY(a = ts_stack_top(vm->stack));
      TS_TRY(a->type == TSTYPE_DOUBLE);
      switch(ins->type) {
      case TSTYPE_VARS:
	varid = ts_var_mgr_get_var_id(vm->cur_table->var_mgr, ins->s);
	if(-1 == varid) {
	  TS_TRY(ts_var_mgr_new_var(vm->cur_table->var_mgr, ins->s, &varid));
	}
	TS_TRY(ts_var_mgr_set_var(vm->cur_table->var_mgr, varid, vm->cur_gid, a->lf));
	ins->type = TSTYPE_SVARI;
	ins->vid = varid;
	break;
      case TSTYPE_SVARI:
	TS_TRY(ts_var_mgr_set_var(vm->cur_table->var_mgr, ins->vid, vm->cur_gid, a->lf));
	break;
      default:
	(void)ts_warn(stderr, "\n");
	goto fail;
      }
      break;
    case OP_SETVAR:
      TS_TRY(a = ts_stack_top(vm->stack));
      _ts_vm_pre_do_op1(a, vt1);
      if(ins->type == TSTYPE_DOT) {
	tid = ts_vm_get_table_id(vm, ins->tablename);
	if(-1 == tid) {
	  (void)ts_warn(stderr, "runtime error @[%d|%d]: set var [%s] does not found.\n", vm->l, vm->c, ins->tablename);
	  goto fail;
	}
	varid = ts_table_get_var_id(vm->table_ar[tid], ins->varname);
	ins->tid = tid;
	ins->vid = varid;
	vt2 = ts_table_get_var_type(vm->table_ar[tid], varid);
	TS_TRY(ts_vm_set_var(vm, tid, varid, a));
	switch(vt2) {
	case TSTYPE_INT:
	  ins->opcode = OP_SETVARI;
	  break;
	case TSTYPE_FLOAT:
	  ins->opcode = OP_SETVARF;
	  break;
	case TSTYPE_DOUBLE:
	case TSTYPE_TIME:
	  ins->opcode = OP_SETVARLF;
	  break;
	default:
	  (void)ts_warn(stderr, "\n");
	  goto fail;	  
	}
      } else {
	varid = ts_table_get_var_id(vm->cur_table, ins->s);
	if(-1 == varid) {
	  TS_TRY(ts_table_new_var(vm->cur_table, ins->s, vt1, 0, &varid));
	  vt2 = vt1;
	} else {
	  vt2 = ts_table_get_var_type(vm->cur_table, varid);
	  if(vt2 != vt1 && (vt2 != TSTYPE_DOUBLE && vt1 != TSTYPE_FLOAT)) {
	    (void)ts_warn(stderr, "runtime error @[%d|%d]: type mismatch in assignment.\n", vm->l, vm->c);
	    goto fail;
	  }
	}
	switch(vt2) {
	case TSTYPE_INT:
	  ins->opcode = OP_SETVARI;
	  TS_TRY(ts_table_set_vari(vm->cur_table, varid, vm->cur_gid, a->i));
	  break;
	case TSTYPE_FLOAT:
	  TS_TRY(ts_table_set_varf(vm->cur_table, varid, vm->cur_gid, a->f));
	  ins->opcode = OP_SETVARF;
	  break;
	case TSTYPE_DOUBLE:
	case TSTYPE_TIME:
	  if(vt1 == TSTYPE_FLOAT) {
	    TS_TRY(ts_table_set_varlf(vm->cur_table, varid, vm->cur_gid, a->f));
	  } else {
	    TS_TRY(ts_table_set_varlf(vm->cur_table, varid, vm->cur_gid, a->lf));
	  }
	  ins->opcode = OP_SETVARLF;
	  break;
	default:
	  (void)ts_warn(stderr, "\n");
	  goto fail;
	}
	ins->vid = varid;
	ins->type = TSTYPE_VARI;
	TS_TRY(ts_prgm_append_new_varid(p, varid));
      }
      break;
    case OP_SETOUTVAR:
      TS_TRY(a = ts_stack_top(vm->stack));
      _ts_vm_pre_do_op1(a, vt1);
      if(ins->type == TSTYPE_DOT) {
	(void)ts_warn(stderr, "runtime error @[%d|%d]: can not set out var for other table.\n", vm->l, vm->c);
	goto fail;
      }
      varid = ts_table_get_var_id(vm->cur_table, ins->s);
      if(-1 == varid) {
	TS_TRY(ts_table_new_var(vm->cur_table, ins->s, vt1, 1, &varid));
	vt2 = vt1;
      } else {
	vm->cur_table->is_out_var_ar[varid] = 1;
	vt2 = ts_table_get_var_type(vm->cur_table, varid);
	if(vt2 != vt1 && (vt2 != TSTYPE_DOUBLE && vt1 != TSTYPE_FLOAT)) {
	  (void)ts_warn(stderr, "runtime error @[%d|%d]: type mismatch in assignment.\n", vm->l, vm->c);
	  goto fail;
	}
      }
      switch(vt2) {
      case TSTYPE_INT:
	ins->opcode = OP_SETVARI;
	TS_TRY(ts_table_set_vari(vm->cur_table, varid, vm->cur_gid, a->i));
	break;
      case TSTYPE_FLOAT:
	TS_TRY(ts_table_set_varf(vm->cur_table, varid, vm->cur_gid, a->f));
	ins->opcode = OP_SETVARF;
	break;
      case TSTYPE_DOUBLE:
      case TSTYPE_TIME:
	if(vt1 == TSTYPE_FLOAT) {
	  TS_TRY(ts_table_set_varlf(vm->cur_table, varid, vm->cur_gid, a->f));
	} else {
	  TS_TRY(ts_table_set_varlf(vm->cur_table, varid, vm->cur_gid, a->lf));
	}
	ins->opcode = OP_SETVARLF;
	break;
      default:
	(void)ts_warn(stderr, "\n");
	goto fail;
      }
      ins->vid = varid;
      ins->type = TSTYPE_VARI;
      TS_TRY(ts_prgm_append_new_varid(p, varid));
      break;
    case OP_SETVARI:
      TS_TRY(a = ts_stack_top(vm->stack));
      if(a->type == TSTYPE_VARI) {
	TS_TRY(ts_table_get_vari(vm->cur_table, a->vid, vm->cur_gid, &a->i));
	a->type = TSTYPE_INT;
      } else if(a->type == TSTYPE_SVARI) {
	TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, a->vid, vm->cur_gid, &a->lf));
	a->i = a->lf;
	a->type = TSTYPE_INT;
      }
      if(ins->type == TSTYPE_DOT) {
	TS_TRY(ts_vm_set_var(vm, ins->tid, ins->vid, a));
      } else {
	TS_TRY(ts_table_set_vari(vm->cur_table, ins->vid, vm->cur_gid, a->i));
      }
      break;
    case OP_SETVARF:
      TS_TRY(a = ts_stack_top(vm->stack));
      if(a->type == TSTYPE_VARI) {
	TS_TRY(ts_table_get_varf(vm->cur_table, a->vid, vm->cur_gid, &a->f));
	a->type = TSTYPE_FLOAT;
      } else if(a->type == TSTYPE_SVARI) {
	TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, a->vid, vm->cur_gid, &a->lf));
	a->f = a->lf;
	a->type = TSTYPE_FLOAT;
      }
      if(ins->type == TSTYPE_DOT) {
	TS_TRY(ts_vm_set_var(vm, ins->tid, ins->vid, a));
      } else {
	TS_TRY(ts_table_set_varf(vm->cur_table, ins->vid, vm->cur_gid, a->f));
      }
      break;
    case OP_SETVARLF:
      TS_TRY(a = ts_stack_top(vm->stack));
      if(a->type == TSTYPE_VARI) {
	vt1 = ts_table_get_var_type(vm->cur_table, a->vid);
	if(vt1 == TSTYPE_DOUBLE || vt1 == TSTYPE_TIME) {
	  TS_TRY(ts_table_get_varlf(vm->cur_table, a->vid, vm->cur_gid, &a->lf));
	  a->type = vt1;
	} else if(vt1 == TSTYPE_FLOAT) {
	  TS_TRY(ts_table_get_varf(vm->cur_table, a->vid, vm->cur_gid, &tf));
	  a->lf = tf;
	  a->type = TSTYPE_DOUBLE;
	} else {
	  (void)ts_warn(stderr, "runtime error @[%d|%d]: can not set other type to double.\n", vm->l, vm->c);
	  goto fail;
	}
      } else if(a->type == TSTYPE_SVARI) {
	TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, a->vid, vm->cur_gid, &a->lf));
	a->type = TSTYPE_DOUBLE;
      }
      if(ins->type == TSTYPE_DOT) {
	TS_TRY(ts_vm_set_var(vm, ins->tid, ins->vid, a));
      } else {
	if(a->type == TSTYPE_DOUBLE || a->type == TSTYPE_TIME) {
	  TS_TRY(ts_table_set_varlf(vm->cur_table, ins->vid, vm->cur_gid, a->lf));
	} else if(a->type == TSTYPE_FLOAT) {
	  TS_TRY(ts_table_set_varlf(vm->cur_table, ins->vid, vm->cur_gid, a->f));
	} else {
	  (void)ts_warn(stderr, "runtime error @[%d|%d]: can not set other type to double.\n", vm->l, vm->c);
	  goto fail;
	}
      }
      break;
    default:
      (void)ts_warn(stderr, "Illegal instruction %s.\n", nodetype_to_str(ins->opcode));
      goto fail;
    }
    //printf("%d\n", vm->stack->top);
  }
  TS_TRY(vm->stack->top == top+1);
  return 1;
 fail:
  return 0;
}

short ts_vm_get_table_id(ts_vm_t *vm, const char *tablename)
{
  TS_TRY(vm && tablename);
  ts_table_meta_t *ptable_meta = 0;
  ptable_meta = ts_hash_table_get(vm->table_meta_table, tablename);
  if(ptable_meta) {
    return ptable_meta->id;
  } else {
    return -1;
  }
 fail:
  return -1;
}

int ts_vm_get_var_valuelf(ts_vm_t *vm, ts_var_mgr_t *var_mgr, ts_stack_elem_t *a, double *v)
{
  TS_TRY(a && v);
  short varid;
  ts_type_t vt;
  switch(a->type) {							
  case TSTYPE_VARS:							
    varid = ts_table_get_var_id(vm->cur_table, a->s);		
    if(-1 == varid) {							
      varid = ts_var_mgr_get_var_id(var_mgr, a->s);		
      if(-1 == varid) {						
	(void)ts_warn(stderr, "runtime error @[%d|%d]:var [%s] not found.\n", vm->l, vm->c, a->s); 
	goto fail;							
      }								
      TS_TRY(ts_var_mgr_get_var(var_mgr, varid, vm->cur_gid, v)); 
      break;								
    } else {								
      a->vid = varid;						
    }									
  case TSTYPE_VARI:							
    vt = ts_table_get_var_type(vm->cur_table, a->vid);		
    switch(vt) {							
    case TSTYPE_INT:					        	
      TS_TRY(ts_table_get_vari(vm->cur_table, a->vid, vm->cur_gid, &a->i)); 
      *v = a->i;
      break;								
    case TSTYPE_FLOAT:						
      TS_TRY(ts_table_get_varf(vm->cur_table, a->vid, vm->cur_gid, &a->f)); 
      *v = a->f;
      break;								
    case TSTYPE_DOUBLE:						
    case TSTYPE_TIME:							
      TS_TRY(ts_table_get_varlf(vm->cur_table, a->vid, vm->cur_gid, &a->lf)); 
      *v = a->lf;
      break;								
    default:								
      (void)ts_warn(stderr, "\n");					
      goto fail;							
    }									
    break;								
  case TSTYPE_SVARI:							
    TS_TRY(ts_var_mgr_get_var(var_mgr, a->vid, vm->cur_gid, v)); 
    break;								
  case TSTYPE_INT:					        	
    *v = a->i;
    break;								
  case TSTYPE_FLOAT:						
    *v = a->f;
    break;								
  case TSTYPE_DOUBLE:						
  case TSTYPE_TIME:							
    *v = a->lf;
    break;								
  default:								
    (void)ts_warn(stderr, "\n");					
    goto fail;
  }
  									
  return 1;
 fail:
  return 0;
}

int ts_vm_get_var_valuef(ts_vm_t *vm, ts_var_mgr_t *var_mgr, ts_stack_elem_t *a, float *v)
{
  TS_TRY(a && v);
  short varid;
  ts_type_t vt;
  double vlf;
  switch(a->type) {							
  case TSTYPE_VARS:							
    varid = ts_table_get_var_id(vm->cur_table, a->s);		
    if(-1 == varid) {							
      varid = ts_var_mgr_get_var_id(var_mgr, a->s);		
      if(-1 == varid) {						
	(void)ts_warn(stderr, "runtime error @[%d|%d]:var [%s] not found.\n", vm->l, vm->c, a->s);
	goto fail;							
      }								
      TS_TRY(ts_var_mgr_get_var(var_mgr, varid, vm->cur_gid, &vlf)); 
      *v = vlf;
      break;								
    } else {								
      a->vid = varid;						
    }									
  case TSTYPE_VARI:							
    vt = ts_table_get_var_type(vm->cur_table, a->vid);		
    switch(vt) {							
    case TSTYPE_INT:					        	
      TS_TRY(ts_table_get_vari(vm->cur_table, a->vid, vm->cur_gid, &a->i)); 
      *v = a->i;
      break;								
    case TSTYPE_FLOAT:						
      TS_TRY(ts_table_get_varf(vm->cur_table, a->vid, vm->cur_gid, &a->f)); 
      *v = a->f;
      break;								
    case TSTYPE_DOUBLE:						
    case TSTYPE_TIME:							
      TS_TRY(ts_table_get_varlf(vm->cur_table, a->vid, vm->cur_gid, &a->lf)); 
      *v = a->lf;
      break;								
    default:								
      (void)ts_warn(stderr, "\n");					
      goto fail;							
    }									
    break;								
  case TSTYPE_SVARI:							
    TS_TRY(ts_var_mgr_get_var(var_mgr, a->vid, vm->cur_gid, &vlf)); 
    *v = vlf;
    break;								
  case TSTYPE_INT:					        	
    *v = a->i;
    break;								
  case TSTYPE_FLOAT:						
    *v = a->f;
    break;								
  case TSTYPE_DOUBLE:						
  case TSTYPE_TIME:							
    *v = a->lf;
    break;								
  default:								
    (void)ts_warn(stderr, "\n");					
    goto fail;
  }
  									
  return 1;
 fail:
  return 0;
}

int ts_vm_get_var_valuei(ts_vm_t *vm, ts_var_mgr_t *var_mgr, ts_stack_elem_t *a, int *v)
{
  TS_TRY(a && v);
  short varid;
  ts_type_t vt;
  double vlf;
  switch(a->type) {							
  case TSTYPE_VARS:							
    varid = ts_table_get_var_id(vm->cur_table, a->s);		
    if(-1 == varid) {							
      varid = ts_var_mgr_get_var_id(var_mgr, a->s);		
      if(-1 == varid) {						
	(void)ts_warn(stderr, "runtime error @[%d|%d]:var [%s] not found.\n", vm->l, vm->c, a->s);
	goto fail;							
      }								
      TS_TRY(ts_var_mgr_get_var(var_mgr, varid, vm->cur_gid, &vlf)); 
      *v = vlf;
      break;								
    } else {								
      a->vid = varid;						
    }									
  case TSTYPE_VARI:							
    vt = ts_table_get_var_type(vm->cur_table, a->vid);		
    switch(vt) {							
    case TSTYPE_INT:					        	
      TS_TRY(ts_table_get_vari(vm->cur_table, a->vid, vm->cur_gid, &a->i)); 
      *v = a->i;
      break;								
    case TSTYPE_FLOAT:						
      TS_TRY(ts_table_get_varf(vm->cur_table, a->vid, vm->cur_gid, &a->f)); 
      *v = a->f;
      break;								
    case TSTYPE_DOUBLE:						
    case TSTYPE_TIME:							
      TS_TRY(ts_table_get_varlf(vm->cur_table, a->vid, vm->cur_gid, &a->lf)); 
      *v = a->lf;
      break;								
    default:								
      (void)ts_warn(stderr, "\n");					
      goto fail;							
    }									
    break;								
  case TSTYPE_SVARI:							
    TS_TRY(ts_var_mgr_get_var(var_mgr, a->vid, vm->cur_gid, &vlf)); 
    *v = vlf;
    break;								
  case TSTYPE_INT:					        	
    *v = a->i;
    break;								
  case TSTYPE_FLOAT:						
    *v = a->f;
    break;								
  case TSTYPE_DOUBLE:						
  case TSTYPE_TIME:							
    *v = a->lf;
    break;								
  default:								
    (void)ts_warn(stderr, "\n");					
    goto fail;
  }
  									
  return 1;
 fail:
  return 0;
}

int ts_vm_table_load(ts_vm_t *vm, char *tablename, ts_table_t **restbl, short *resid)
{
  TS_TRY(vm && tablename && resid);
  short tid = ts_vm_get_table_id(vm, tablename);
  if(-1 != tid) {
    if(vm->table_ar[tid]) {
      // already loaded
      if(restbl)
	*restbl = vm->table_ar[tid];
      *resid = tid;
      return 1;
    }    
  }
  ts_table_t *t;
  TS_TRY(t = ts_table_read(tablename));
  if(vm->ntable == vm->mtable) {
    TS_TRY(vm->table_ar = realloc(vm->table_ar, (vm->mtable+100)*sizeof(ts_table_t*)));
    memset(&vm->table_ar[vm->mtable], 0, 100*sizeof(ts_table_t*));
    vm->mtable += 100;
  }
  vm->table_ar[vm->ntable] = t;
  TS_TRY(ts_hash_table_insert(vm->table_meta_table, tablename, t->meta));
  if(restbl)
    *restbl = t;
  *resid = vm->ntable;
  vm->ntable++;

  return 1;
 fail:
  return 0;
}

int ts_vm_set_var(ts_vm_t *vm, short tid, short vid, ts_stack_elem_t *v)
{
  TS_TRY(vm && v && tid >= 0 && tid <vm->ntable);
  ts_table_t *t = vm->table_ar[tid], *ct = vm->cur_table;
#ifdef DEBUG
  TS_TRY(vid >= 0 && vid < t->nttlcol);
#endif
  ////////////////////////////
  int cgid = vm->cur_gid;
  int gid = ts_table_translate_gid(ct, t, cgid);
  ts_time_t *tcol = t->p_col_ar[t->tcol][gid];
  ts_time_t ctime = ((ts_time_t*)ct->p_col_ar[ct->tcol][cgid])[ct->cur_row_ar[cgid]];
  int i, n = t->nrow_ar[gid], start = t->cur_row_ar[gid];
  for(i=start;i<n;i++) {
    if(tcol[i] > ctime + 1e-9) {
      if(i-1 >= start) {
	i--;
	break;
      } else {
	(void)ts_warn(stderr, "runtime error @[%d|%d]: join table time inconsistence.\n", vm->l, vm->c);
	goto fail;	
      }
    } else if(fabs(tcol[i]-ctime) < 1e-9) {
      break;
    }
  }
  if(i == n) {
    i--;
  }
  t->cur_row_ar[gid] = i;
  ////////////////////////////
  ts_type_t vt = ts_table_get_var_type(t, vid);
  int ti;
  float tf;
  double tlf;
  switch(vt) {
  case TSTYPE_INT:
    TS_TRY(ts_vm_get_var_valuei(vm, vm->cur_table->var_mgr, v, &ti));
    TS_TRY(ts_table_set_vari(t, vid, gid, ti));
    break;
  case TSTYPE_FLOAT:
    TS_TRY(ts_vm_get_var_valuef(vm, vm->cur_table->var_mgr, v, &tf));
    TS_TRY(ts_table_set_varf(t, vid, gid, tf));
    break;
  case TSTYPE_DOUBLE:
  case TSTYPE_TIME:
    TS_TRY(ts_vm_get_var_valuelf(vm, vm->cur_table->var_mgr, v, &tlf));
    TS_TRY(ts_table_set_varlf(t, vid, gid, tlf));
    break;
  default:
    (void)ts_warn(stderr, "runtime error @[%d|%d]: get other table's var type error.\n", vm->l, vm->c);
    goto fail;
  }

  return 1;
 fail:
  return 0;
}

int ts_vm_get_var(ts_vm_t *vm, short tid, short vid, ts_stack_elem_t *v)
{
  TS_TRY(vm && v && tid >=0 && tid < vm->ntable);
  ts_table_t *t = vm->table_ar[tid], *ct = vm->cur_table;
#ifdef DEBUG
  TS_TRY(vid >= 0 && vid < t->nttlcol);
#endif
  int cgid = vm->cur_gid;
  int gid = ts_table_translate_gid(ct, t, cgid);
  ts_time_t *tcol = t->p_col_ar[t->tcol][gid];
  ts_time_t ctime = ((ts_time_t*)ct->p_col_ar[ct->tcol][cgid])[ct->cur_row_ar[cgid]];
  int i, n = t->nrow_ar[gid], start = t->cur_row_ar[gid];
  for(i=start;i<n;i++) {
    if(tcol[i] > ctime + 1e-9) {
      if(i-1 >= start) {
	i--;
	break;
      } else {
	(void)ts_warn(stderr, "runtime error @[%d|%d]: join table time inconsistence.\n", vm->l, vm->c);
	goto fail;
      }
    } else if(fabs(tcol[i]-ctime) < 1e-9) {
      break;
    }
  }
  if(i == n) {
    i--;
  }
  t->cur_row_ar[gid] = i;
  
  ts_type_t vt = ts_table_get_var_type(t, vid);
  switch(vt) {
  case TSTYPE_INT:
    TS_TRY(ts_table_get_vari(t, vid, gid, &v->i));
    break;
  case TSTYPE_FLOAT:
    TS_TRY(ts_table_get_varf(t, vid, gid, &v->f));
    break;
  case TSTYPE_DOUBLE:
  case TSTYPE_TIME:
    TS_TRY(ts_table_get_varlf(t, vid, gid, &v->lf));
    break;
  default:
    (void)ts_warn(stderr, "runtime error @[%d|%d]: get other table's var type error.\n", vm->l, vm->c);
    goto fail;
  }

  v->type = vt;
  return 1;
 fail:
  return 0;
}

int ts_vm_clear(ts_vm_t *vm)
{
  TS_TRY(vm);
  TS_TRY(ts_stack_clear(vm->stack));
  int i,j;
  for(i=0;i<vm->nfunc;i++) {
    if(vm->func_store_ar[i]) {
      for(j=0;j<vm->ngrp;j++) {
	ts_free(vm->func_store_ar[i][j]);
      }
      ts_free(vm->func_store_ar[i]);
    }
  }
  vm->nfunc = 0;

  for(i=0;i<vm->ntable;i++) {
    TS_TRY(ts_table_clear(vm->table_ar[i]));
  }
  
  return 1;
 fail:
  return 0;
}

