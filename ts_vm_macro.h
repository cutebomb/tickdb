#ifndef TS_VM_MACRO_H
#define TS_VM_MACRO_H


#define _ts_vm_pre_do_op2(a, b, vt1, vt2)  {				\
    TS_TRY(ts_stack_pop_to(vm->stack, b));				\
    TS_TRY(a = ts_stack_top(vm->stack));				\
    switch((a)->type) {							\
    case TSTYPE_VARS:							\
      varid = ts_table_get_var_id(vm->cur_table, (a)->s);		\
      if(-1 == varid) {							\
	varid = ts_var_mgr_get_var_id(vm->cur_table->var_mgr, (a)->s);	\
	if(-1 == varid) {						\
	  (void)ts_warn(stderr, "runtime error @[%d|%d]:var [%s] not found.\n", vm->l, vm->c, (a)->s); \
	  goto fail;							\
	}								\
	TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, varid, vm->cur_gid, &(a)->lf)); \
	vt1 = TSTYPE_DOUBLE;						\
	(a)->type = TSTYPE_DOUBLE;					\
	break;								\
      } else {								\
	(a)->vid = varid;						\
      }									\
    case TSTYPE_VARI:							\
      vt1 = ts_table_get_var_type(vm->cur_table, (a)->vid);		\
      (a)->type = vt1;			                		\
      switch((vt1)) {							\
      case TSTYPE_INT:					        	\
	TS_TRY(ts_table_get_vari(vm->cur_table, (a)->vid, vm->cur_gid, &(a)->i)); \
	break;								\
      case TSTYPE_FLOAT:						\
	TS_TRY(ts_table_get_varf(vm->cur_table, (a)->vid, vm->cur_gid, &(a)->f)); \
	break;								\
      case TSTYPE_DOUBLE:						\
      case TSTYPE_TIME:							\
	TS_TRY(ts_table_get_varlf(vm->cur_table, (a)->vid, vm->cur_gid, &(a)->lf)); \
	break;								\
      case TSTYPE_SYM:							\
	TS_TRY(ts_table_get_varsym(vm->cur_table, (a)->vid, vm->cur_gid, &(a)->s)); \
	break;								\
      default:								\
	(void)ts_warn(stderr, "\n");					\
	goto fail;							\
      }									\
      break;								\
    case TSTYPE_SVARI:							\
      TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, (a)->vid, vm->cur_gid, &(a)->lf)); \
      vt1 = TSTYPE_DOUBLE;						\
      (a)->type = TSTYPE_DOUBLE;					\
      break;								\
    default:								\
      vt1 = (a)->type;							\
      break;								\
    }									\
    switch((b)->type) {							\
    case TSTYPE_VARS:							\
      varid = ts_table_get_var_id(vm->cur_table, (b)->s);		\
      if(-1 == varid) {							\
	varid = ts_var_mgr_get_var_id(vm->cur_table->var_mgr, (b)->s);	\
	if(-1 == varid) {						\
	  (void)ts_warn(stderr, "runtime error @[%d|%d]:var [%s] not found.\n", vm->l, vm->c, (b)->s); \
	  goto fail;							\
	}								\
	TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, varid, vm->cur_gid, &(b)->lf)); \
	vt2 = TSTYPE_DOUBLE;						\
	(b)->type = TSTYPE_DOUBLE;					\
	break;								\
      } else {								\
	(b)->vid = varid;						\
      }									\
    case TSTYPE_VARI:							\
      vt2 = ts_table_get_var_type(vm->cur_table, (b)->vid);		\
      (b)->type = vt2;			                		\
      switch((vt2)) {							\
      case TSTYPE_INT:					        	\
	TS_TRY(ts_table_get_vari(vm->cur_table, (b)->vid, vm->cur_gid, &(b)->i)); \
	break;								\
      case TSTYPE_FLOAT:						\
	TS_TRY(ts_table_get_varf(vm->cur_table, (b)->vid, vm->cur_gid, &(b)->f)); \
	break;								\
      case TSTYPE_DOUBLE:						\
      case TSTYPE_TIME:							\
	TS_TRY(ts_table_get_varlf(vm->cur_table, (b)->vid, vm->cur_gid, &(b)->lf)); \
	break;								\
      case TSTYPE_SYM:							\
	TS_TRY(ts_table_get_varsym(vm->cur_table, (b)->vid, vm->cur_gid, &(b)->s)); \
	break;								\
      default:								\
	(void)ts_warn(stderr, "\n");					\
	goto fail;							\
      }									\
      break;								\
    case TSTYPE_SVARI:							\
      TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, (b)->vid, vm->cur_gid, &(b)->lf)); \
      vt2 = TSTYPE_DOUBLE;						\
      (b)->type = TSTYPE_DOUBLE;					\
      break;								\
    default:								\
      vt2 = (b)->type;							\
      break;								\
    }									\
  }									\


#define _ts_vm_pre_do_op1(a, vt) {					\
    switch((a)->type) {							\
    case TSTYPE_VARS:							\
      varid = ts_table_get_var_id(vm->cur_table, (a)->s);		\
      if(-1 == varid) {							\
	varid = ts_var_mgr_get_var_id(vm->cur_table->var_mgr, (a)->s);	\
	if(-1 == varid) {						\
	  (void)ts_warn(stderr, "runtime error @[%d|%d]:var [%s] not found.\n", vm->l, vm->c, (a)->s); \
	  goto fail;							\
	}								\
	TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, varid, vm->cur_gid, &(a)->lf)); \
	vt = TSTYPE_DOUBLE;						\
	(a)->type = TSTYPE_DOUBLE;					\
	break;								\
      } else {								\
	(a)->vid = varid;						\
      }									\
    case TSTYPE_VARI:							\
      vt = ts_table_get_var_type(vm->cur_table, (a)->vid);		\
      (a)->type = vt;			                		\
      switch((vt)) {							\
      case TSTYPE_INT:					        	\
	TS_TRY(ts_table_get_vari(vm->cur_table, (a)->vid, vm->cur_gid, &(a)->i)); \
	break;								\
      case TSTYPE_FLOAT:						\
	TS_TRY(ts_table_get_varf(vm->cur_table, (a)->vid, vm->cur_gid, &(a)->f)); \
	break;								\
      case TSTYPE_DOUBLE:						\
      case TSTYPE_TIME:							\
	TS_TRY(ts_table_get_varlf(vm->cur_table, (a)->vid, vm->cur_gid, &(a)->lf)); \
	break;								\
      case TSTYPE_SYM:							\
	TS_TRY(ts_table_get_varsym(vm->cur_table, (a)->vid, vm->cur_gid, &(a)->s)); \
	break;								\
      default:								\
	(void)ts_warn(stderr, "\n");					\
	goto fail;							\
      }									\
      break;								\
    case TSTYPE_SVARI:							\
      TS_TRY(ts_var_mgr_get_var(vm->cur_table->var_mgr, (a)->vid, vm->cur_gid, &(a)->lf)); \
      vt = TSTYPE_DOUBLE;						\
      (a)->type = TSTYPE_DOUBLE;					\
      break;								\
    default:								\
      vt = (a)->type;							\
      break;								\
    }									\
  }									\

#define _ts_vm_do_op1(res, a, vta, op, T)  {	\
    switch(vta) {				\
    case TSTYPE_INT:				\
      (res)->i = op (a)->i;			\
      break;					\
    case TSTYPE_FLOAT:				\
      (res)->f = op (a)->f;			\
      break;					\
    case TSTYPE_DOUBLE:				\
    case TSTYPE_TIME:				\
      (res)->lf = op (a)->lf;			\
      break;					\
    default:					\
      (void)ts_warn(stderr, "\n");		\
      goto fail;				\
    }						\
    (res)->type = T;				\
  }						\

#define _ts_vm_do_op1_logical(res, a, vta, op) {	\
    switch(vta) {					\
    case TSTYPE_INT:					\
      (res)->i = op (a)->i;				\
      break;						\
    case TSTYPE_FLOAT:					\
      (res)->i = op (a)->f;				\
      break;						\
    case TSTYPE_DOUBLE:					\
    case TSTYPE_TIME:					\
      (res)->i = op (a)->lf;				\
      break;						\
    default:						\
      (void)ts_warn(stderr, "\n");			\
      goto fail;					\
    }							\
    (res)->type = TSTYPE_INT;				\
  }							\

#define _ts_vm_do_op2(res, a, b, vta, vtb, op, T) {	\
    switch(vta){					\
    case TSTYPE_INT:					\
      switch(vtb) {					\
      case TSTYPE_INT:					\
	(res)->i = (a)->i op (b)->i;			\
	break;						\
      case TSTYPE_FLOAT:				\
	(res)->f = (a)->i op (b)->f;			\
	break;						\
      case TSTYPE_DOUBLE:				\
      case TSTYPE_TIME:					\
	(res)->lf = (a)->i op (b)->lf;			\
	break;						\
      default:						\
	(void)ts_warn(stderr, "\n");			\
	goto fail;					\
      }							\
      break;						\
    case TSTYPE_FLOAT:					\
      switch(vtb) {					\
      case TSTYPE_INT:					\
	(res)->f = (a)->f op (b)->i;			\
	break;						\
      case TSTYPE_FLOAT:				\
	(res)->f = (a)->f op (b)->f;			\
	break;						\
      case TSTYPE_DOUBLE:				\
      case TSTYPE_TIME:					\
	(res)->lf = (a)->f op (b)->lf;			\
	break;						\
      default:						\
	(void)ts_warn(stderr, "\n");			\
	goto fail;					\
      }							\
      break;						\
    case TSTYPE_DOUBLE:					\
    case TSTYPE_TIME:					\
      switch(vtb) {					\
      case TSTYPE_INT:					\
	(res)->lf = (a)->lf op (b)->i;			\
	break;						\
      case TSTYPE_FLOAT:				\
	(res)->lf = (a)->lf op (b)->f;			\
	break;						\
      case TSTYPE_DOUBLE:				\
      case TSTYPE_TIME:					\
	(res)->lf = (a)->lf op (b)->lf;			\
	break;						\
      default:						\
	(void)ts_warn(stderr, "\n");			\
	goto fail;					\
      }							\
      break;						\
    default:						\
      (void)ts_warn(stderr, "\n");			\
      goto fail;					\
    }							\
    (res)->type = T;					\
  }							\


#define _ts_vm_do_func_op2(res, a, b, vta, vtb, func, T) {	\
    switch(vta){						\
    case TSTYPE_INT:						\
      switch(vtb) {						\
      case TSTYPE_INT:						\
	(res)->i = func((a)->i , (b)->i);			\
	break;							\
      case TSTYPE_FLOAT:					\
	(res)->f = func((a)->i , (b)->f);			\
	break;							\
      case TSTYPE_DOUBLE:					\
      case TSTYPE_TIME:						\
	(res)->lf = func((a)->i , (b)->lf);			\
	break;							\
      default:							\
	(void)ts_warn(stderr, "\n");				\
	goto fail;						\
      }								\
      break;							\
    case TSTYPE_FLOAT:						\
      switch(vtb) {						\
      case TSTYPE_INT:						\
	(res)->f = func((a)->f , (b)->i);			\
	break;							\
      case TSTYPE_FLOAT:					\
	(res)->f = func((a)->f , (b)->f);			\
	break;							\
      case TSTYPE_DOUBLE:					\
      case TSTYPE_TIME:						\
	(res)->lf = func((a)->f , (b)->lf);			\
	break;							\
      default:							\
	(void)ts_warn(stderr, "\n");				\
	goto fail;						\
      }								\
      break;							\
    case TSTYPE_DOUBLE:						\
    case TSTYPE_TIME:						\
      switch(vtb) {						\
      case TSTYPE_INT:						\
	(res)->lf = func((a)->lf , (b)->i);			\
	break;							\
      case TSTYPE_FLOAT:					\
	(res)->lf = func((a)->lf , (b)->f);			\
	break;							\
      case TSTYPE_DOUBLE:					\
      case TSTYPE_TIME:						\
	(res)->lf = func((a)->lf , (b)->lf);			\
	break;							\
      default:							\
	(void)ts_warn(stderr, "\n");				\
	goto fail;						\
      }								\
      break;							\
    default:							\
      (void)ts_warn(stderr, "\n");				\
      goto fail;						\
    }								\
    (res)->type = T;						\
  }								\

#define _ts_vm_do_op2_logical(res, a, b, vta, vtb, op) {	\
    (res)->type = TSTYPE_INT;					\
    switch(vta){						\
    case TSTYPE_INT:						\
      switch(vtb) {						\
      case TSTYPE_INT:						\
	(res)->i = (a)->i op (b)->i;				\
	break;							\
      case TSTYPE_FLOAT:					\
	(res)->i = (a)->i op (b)->f;				\
	break;							\
      case TSTYPE_DOUBLE:					\
      case TSTYPE_TIME:						\
	(res)->i = (a)->i op (b)->lf;				\
	break;							\
      default:							\
	(void)ts_warn(stderr, "\n");				\
	goto fail;						\
      }								\
      break;							\
    case TSTYPE_FLOAT:						\
      switch(vtb) {						\
      case TSTYPE_INT:						\
	(res)->i = (a)->f op (b)->i;				\
	break;							\
      case TSTYPE_FLOAT:					\
	(res)->i = (a)->f op (b)->f;				\
	break;							\
      case TSTYPE_DOUBLE:					\
      case TSTYPE_TIME:						\
	(res)->i = (a)->f op (b)->lf;				\
	break;							\
      default:							\
	(void)ts_warn(stderr, "\n");				\
	goto fail;						\
      }								\
      break;							\
    case TSTYPE_DOUBLE:						\
    case TSTYPE_TIME:						\
      switch(vtb) {						\
      case TSTYPE_INT:						\
	(res)->i = (a)->lf op (b)->i;				\
	break;							\
      case TSTYPE_FLOAT:					\
	(res)->i = (a)->lf op (b)->f;				\
	break;							\
      case TSTYPE_DOUBLE:					\
      case TSTYPE_TIME:						\
	(res)->i = (a)->lf op (b)->lf;				\
	break;							\
      default:							\
	(void)ts_warn(stderr, "\n");				\
	goto fail;						\
      }								\
      break;							\
    default:							\
      (void)ts_warn(stderr, "\n");				\
      goto fail;						\
    } }								\

#endif
