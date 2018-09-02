#include "ts_prgm.h"
#include "ts_list.h"
#include <stdlib.h>

#ifdef DEBUG
static int _ts_prgm_print(instruction_t *prgm, int n);
#endif
static int ts_prgm_byte_code_gen(ts_tree_node_t *tn, ts_prgm_t *prgm);
static int _ts_prgm_optimize(ts_tree_node_t *tn, ts_list_elem_t **list);
static int _ts_prgm_append_str(ts_prgm_t *p, char *str);

ts_prgm_t *ts_prgm_new(void)
{
  ts_prgm_t *p;
  TS_TRY(p = calloc(1, sizeof(ts_prgm_t)));

  p->mins = 100;
  p->nins = 0;
  TS_TRY(p->prgm_ar = calloc(p->mins, sizeof(instruction_t)));
  
  p->mvar = 20;
  p->nvar = 0;
  TS_TRY(p->varid_ar = calloc(p->mvar, sizeof(int)));

  return p;
 fail:
  return 0;
}

int ts_prgm_free(ts_prgm_t *prgm)
{
  TS_TRY(prgm);
  ts_free(prgm->varid_ar);
  ts_free(prgm->prgm_ar);
  ts_free(prgm->name);
  int i;
  for(i=0;i<prgm->ns;i++) {
    ts_free(prgm->str_ar[i]);
  }
  ts_free(prgm->str_ar);
  ts_free(prgm->svarid_ar);
  for(i=0;i<prgm->argc;i++) {
    ts_free(prgm->argvname[i]);
  }
  ts_free(prgm->argvname);
  free(prgm);
  return 1;
 fail:
  return 0;
}

int ts_prgm_append_new_varid(ts_prgm_t *p, int vid)
{
  TS_TRY(p);

  if(p->nvar == p->mvar) {
    TS_TRY(p->varid_ar = realloc(p->varid_ar, (p->mvar+10)*sizeof(int)));
    memset(&p->varid_ar[p->nvar], 0, 10*sizeof(int));
    p->mvar += 10;
  }
  
  p->varid_ar[p->nvar] = vid;
  p->nvar++;
  return 1;
 fail:
  return 0;
}

int ts_prgm_clear(ts_prgm_t *prgm)
{
  TS_TRY(prgm);
  prgm->nins = 0;
  int i;
  for(i=0;i<prgm->ns;i++) {
    ts_free(prgm->str_ar[i]);
  }
  prgm->ns = 0;
  return 1;
 fail:
  return 0;
}

#ifdef DEBUG
int ts_prgm_print(ts_prgm_t *prgm)
{
  TS_TRY(prgm);

  if(prgm->nins > 0) {
    TS_TRY(_ts_prgm_print(prgm->prgm_ar, prgm->nins));
  }
  return 1;
 fail:
  return 0;
}

int _ts_prgm_print(instruction_t *prgm, int n)
{
  TS_TRY(prgm && n > 0);
  int i=0;
  for(i=0;i<n;i++) {
    instruction_t *c = &prgm[i];
    switch(c->type) {
    case TSTYPE_INT:
      printf("%d:\t%s int %d", c->ln, nodetype_to_str(c->opcode), c->i);
      break;
    case TSTYPE_FLOAT:
      printf("%d:\t%s float %f", c->ln, nodetype_to_str(c->opcode), c->f);
      break;
    case TSTYPE_DOUBLE:
      printf("%d:\t%s double %lf", c->ln, nodetype_to_str(c->opcode), c->lf);
      break;
    case TSTYPE_STR:
      printf("%d:\t%s str %s", c->ln, nodetype_to_str(c->opcode), c->s);
      break;
    case TSTYPE_VARS:
      printf("%d:\t%s var %s", c->ln, nodetype_to_str(c->opcode), c->s);
      break;
    case TSTYPE_VARI:
      printf("%d:\t%s varid %d", c->ln, nodetype_to_str(c->opcode), c->i);
      break;
    case TSTYPE_TIME:
      printf("%d:\t%s time %s", c->ln, nodetype_to_str(c->opcode), ts_time_to_str(c->t));
      break;
    case TSTYPE_DOT:
      printf("%d:\t%s %s.%s", c->ln, nodetype_to_str(c->opcode), c->tablename, c->varname);
      break;
    default:
      printf("%d:\t%s", c->ln, nodetype_to_str(c->opcode));
      break;
    }
       
    if(c->opcode == OP_DOT) {
      printf(" %s %s", c->tablename, c->varname);
    }
    
    printf("\n");
  }
  return 1;
 fail:
  return 0;
}
#endif

int ts_prgm_byte_code_generate(ts_tree_node_t *tn, ts_prgm_t *prgm)
{
  TS_TRY(tn && prgm);

  TS_TRY(ts_prgm_byte_code_gen(tn, prgm));
  return 1;
 fail:
  return 0;
}

int ts_prgm_byte_code_gen(ts_tree_node_t *tn, ts_prgm_t *prgm)
{
  TS_TRY(tn && prgm);
  int i=0, c, e;
  int ti;
  float tf;
  double tlf;
  instruction_t *t;
  ts_tree_node_t *ctn;
  switch(tn->type) {
  case NODE_PRGM:
  case NODE_STMTS:
    if(tn->nchild>0) {
      ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
      for(i=1;i<tn->nchild;i++) {
	ts_prgm_append(prgm, OP_POP, TSTYPE_NONE, 0, 0, tn->l, tn->c);
	ts_prgm_byte_code_gen(tn->child_ar[i], prgm);
      }
    }
    break;
  case NODE_FORMULA:
    if(tn->child_ar) {
      TS_TRY(tn->nchild == 2);
      ctn = tn->child_ar[0];
      TS_TRY(ctn->type == NODE_PARA_LIST);
      TS_TRY(ts_prgm_gen_arglist(prgm, ctn));
      ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    }
    break;
  case NODE_IFELSE:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm); // cond
    e = prgm->nins;
    ts_prgm_append(prgm, OP_JMPF, TSTYPE_INT, &e, 0, tn->l, tn->c);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm); // do true block
    c = prgm->nins;
    ts_prgm_append(prgm, OP_JMP, TSTYPE_INT, &e, 0, tn->l, tn->c);
    prgm->prgm_ar[e].i = prgm->nins;
    ts_prgm_byte_code_gen(tn->child_ar[2], prgm); // do else (false) block
    prgm->prgm_ar[c].i = prgm->nins;   
    break;
  case NODE_ADD:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_ADD, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_MINUS:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_MINUS, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_MUL:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_MUL, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_DIV:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_DIV, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_MOD:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_MOD, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_GT:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_GT, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_LT:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_LT, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_EQ:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_EQ, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_NEQ:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_NEQ, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_LE:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_LE, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_GE:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_GE, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_AND:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_AND, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_OR:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_OR, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_DOT:
    t = &prgm->prgm_ar[prgm->nins];
    ts_prgm_append(prgm, OP_DOT, TSTYPE_NONE, tn->child_ar[0]->value, tn->child_ar[1]->value, tn->l, tn->c);
    break;
  case NODE_NOT:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_append(prgm, OP_NOT, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_NEGATIVE:
    ts_prgm_byte_code_gen(tn->child_ar[0], prgm);
    ts_prgm_append(prgm, OP_NEG, TSTYPE_NONE, 0, 0, tn->l, tn->c);
    break;
  case NODE_FUNCTION:
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_PUSH, TSTYPE_INT, &tn->valuei, 0, tn->l, tn->c);
    ts_prgm_append(prgm, OP_PUSH, TSTYPE_INT, &tn->child_ar[1]->nchild, 0, tn->l, tn->c);
    ts_prgm_append(prgm, OP_CALL, TSTYPE_VARS, tn->child_ar[0]->value, 0, tn->l, tn->c);
    break;
  case NODE_PARA_LIST:
    for(i=0;i<tn->nchild;i++) {
      ts_prgm_byte_code_gen(tn->child_ar[i], prgm);
    }
    break;
  case NODE_ASSIGN:
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    if(tn->child_ar[0]->type == NODE_DOT) {
      ctn = tn->child_ar[0];
      ts_prgm_append(prgm, OP_SETVAR, TSTYPE_DOT, ctn->child_ar[0]->value, ctn->child_ar[1]->value, tn->l, tn->c);
    } else {
      ts_prgm_append(prgm, OP_SETVAR, TSTYPE_VARS, tn->child_ar[0]->value, 0, tn->l, tn->c);
    }
    break;
  case NODE_OUTPUT_ASSIGN:
    ts_prgm_byte_code_gen(tn->child_ar[1], prgm);
    ts_prgm_append(prgm, OP_SETOUTVAR, TSTYPE_VARS, tn->child_ar[0]->value, 0, tn->l, tn->c);
    break;
  case NODE_STR:
    ts_prgm_append(prgm, OP_PUSHG, TSTYPE_STR, tn->value, 0, tn->l, tn->c);
    break;
  case NODE_VAR:
    ts_prgm_append(prgm, OP_PUSHG, TSTYPE_VARS, tn->value, 0, tn->l, tn->c);
    break;
  case NODE_INT:
    ti = atoi(tn->value);
    ts_prgm_append(prgm, OP_PUSH, TSTYPE_INT, &ti, 0, tn->l, tn->c);
    break;
  case NODE_FLOAT:
    tf = atof(tn->value);
    ts_prgm_append(prgm, OP_PUSH, TSTYPE_FLOAT, &tf, 0, tn->l, tn->c);
    break;
  case NODE_DOUBLE:
    tlf = atof(tn->value);
    ts_prgm_append(prgm, OP_PUSH, TSTYPE_DOUBLE, &tlf, 0, tn->l, tn->c);
    break;
  case NODE_TIME_CONST:
    tlf = ts_time_const_to_double(tn->value);
    ts_prgm_append(prgm, OP_PUSHG, TSTYPE_TIME, &tlf, 0, tn->l, tn->c);
    break;
  case NODE_EMPTY:
    ti = 0;
    ts_prgm_append(prgm, OP_PUSH, TSTYPE_INT, &ti, 0, tn->l, tn->c);
    break;
  default:    
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  return 1;
 fail:
  return 0;
}

int ts_prgm_optimize(ts_tree_node_t *tn, int funcinstid_start, int *mfuncinstid)
{
  TS_TRY(tn);
  ts_list_elem_t *list=0;
  _ts_prgm_optimize(tn, &list);

  ts_list_elem_t *elem = list;
  int i = funcinstid_start;
  while(elem) {
    ts_list_elem_t *e = elem->value;
    while(e) {
      ts_tree_node_t *tn = e->value;
      tn->valuei = i;
      e = e->next;
    }
    elem = elem->next;
    i++;
  }
  *mfuncinstid = i;
  return 1;
 fail:
  return 0;
}

int _ts_prgm_optimize(ts_tree_node_t *tn, ts_list_elem_t **list)
{
  TS_TRY(tn && list);
  ts_list_elem_t *elem = *list;
  int i=0;
  switch(tn->type) {
  case NODE_FUNCTION:
    while(elem) {
      ts_tree_node_t *ftn = (ts_tree_node_t*)((ts_list_elem_t*)((elem)->value))->value;
      TS_TRY(ftn);
      if(0 == ts_tree_node_eq(ftn, tn)) {
	ts_list_append((ts_list_elem_t**)&(elem)->value, tn, 0);
	goto finish;
      }
      elem = (elem)->next;
    }
    ts_list_elem_t *ne = ts_list_elem_new(tn, 0);
    ts_list_append(list, ne, 0);
    //break; continue to default
  default:
    for(i=0;i<tn->nchild;i++) {
      _ts_prgm_optimize(tn->child_ar[i], list);
    }
    break;
  }
 finish:
  return 1;
 fail:
  return 0;
}

int _ts_prgm_append_str(ts_prgm_t *p, char *str)
{
  TS_TRY(p && str);
  
  if(p->ns == p->ms) {
    ts_realloc(p->str_ar, p->ms, 1000, char*);
    p->ms += 1000;
  }
  p->str_ar[p->ns] = str;
  p->ns++;
  return 1;
 fail:
  return 0;
}

int ts_prgm_append(ts_prgm_t *prgm, opcode_t opcode, ts_type_t type, void *oprand1, void *oprand2, int l, int c)
{
  TS_TRY(prgm && opcode < OP_MAX && type <TSTYPE_MAX);
  
  if(prgm->nins == prgm->mins) {
    ts_realloc(prgm->prgm_ar, prgm->mins, 1000, instruction_t);
    prgm->mins += 1000;
  }
  instruction_t *ins = &prgm->prgm_ar[prgm->nins];

  ins->opcode = opcode;
  ins->type = type;
  if(0 == oprand2) {
  switch(type) {
  case TSTYPE_INT:
    ins->i = *(int*)oprand1;
    break;
  case TSTYPE_FLOAT:
    ins->f = *(float*)oprand1;
    break;
  case TSTYPE_DOUBLE:
    ins->lf = *(double*)oprand1;
    break;
  case TSTYPE_STR:
    ins->s = strdup((char*)oprand1);
    TS_TRY(_ts_prgm_append_str(prgm, ins->s));
    break;
  case TSTYPE_VARS:
    ins->s = strdup((char*)oprand1);
    TS_TRY(_ts_prgm_append_str(prgm, ins->s));
    break;
  case TSTYPE_TIME:
    ins->t = *(ts_time_t*)oprand1;
    break;
  default:
    break;
  }
  } else {
    ins->tablename = strdup((char*)oprand1);
    TS_TRY(_ts_prgm_append_str(prgm, ins->tablename));
    ins->varname = strdup((char*)oprand2);
    TS_TRY(_ts_prgm_append_str(prgm, ins->varname));
  }

#ifdef DEBUG
  ins->ln = prgm->nins;
#endif
  ins->l = l;
  ins->c = c;
  prgm->nins++;
  return 1;
 fail:
  return 0;
}

int ts_prgm_gen_arglist(ts_prgm_t *prgm, ts_tree_node_t *pl)
{
  TS_TRY(prgm && pl);
  prgm->argc = pl->nchild;
  prgm->argvname = calloc(prgm->argc, sizeof(char*));
  prgm->argv = calloc(prgm->argc, sizeof(double));
  prgm->svarid_ar = calloc(prgm->argc, sizeof(short));
  int i;
  for(i=0;i<pl->nchild;i++) {
    if(pl->child_ar[i]->type == NODE_VAR) {
      prgm->argvname[i] = strdup(pl->child_ar[i]->value);
      prgm->argv[i] = 0.0;
    } else if(pl->child_ar[i]->type == NODE_ASSIGN) {
      prgm->argvname[i] = strdup(pl->child_ar[i]->child_ar[0]->value);
      prgm->argv[i] = atof(pl->child_ar[i]->child_ar[1]->value);
    } else {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }      
  }
  return 1;
 fail:
  return 0;
}
