#include "ts_tree.h"
#include "ts_vm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

extern ts_vm_t *vm;

char* nodetype_to_str(int type)
{
  TS_TRY(type < OP_MAX);
  static char *str[] = { "",
			 "NODE_PRGM",
			 "NODE_FORMULA",
			 "NODE_STMTS",
			 "NODE_IFELSE",
			 "NODE_ADD",
			 "NODE_MINUS",
			 "NODE_MUL",
			 "NODE_DIV",
			 "NODE_MOD",
			 "NODE_GT",
			 "NODE_LT",
			 "NODE_EQ",
			 "NODE_NEQ",
			 "NODE_LE",
			 "NODE_GE",
			 "NODE_AND",
			 "NODE_OR",
			 "NODE_DOT",
			 "NODE_NOT",
			 "NODE_NEGATIVE",
			 "NODE_OUTPUT_ASSIGN",
			 "NODE_ASSIGN", 
			 "NODE_NEWOUTVAR",
			 "NODE_NEWVAR",
			 "NODE_FUNCTION",
			 "NODE_PARA_LIST",
			 "NODE_STR",
			 "NODE_VAR",
			 "NODE_INT", 
			 "NODE_FLOAT", 
			 "NODE_DOUBLE",
			 "NODE_TIME_CONST",
			 "NODE_EMPTY",};
  static char *opstr[] = {  "OP_NOP",
			    "OP_PUSHG",
			    "OP_PUSH",
			    "OP_POP",
			    "OP_JMP",
			    "OP_JMPF",
			    "OP_ADD",
			    "OP_MINUS",
			    "OP_MUL",
			    "OP_DIV",
			    "OP_MOD",
			    "OP_AND",
			    "OP_OR",
			    "OP_LT",
			    "OP_GT",
			    "OP_EQ",
			    "OP_NEQ",
			    "OP_LE",
			    "OP_GE",
			    "OP_DOT",
			    "OP_DOTI",
			    "OP_DOTT",
			    "OP_NOT",
			    "OP_NEG",
			    "OP_CALL",
			    "OP_CALLI",
			    "OP_SETVAR",
			    "OP_SETSVAR",
			    "OP_SETVARI",
			    "OP_SETVARF",
			    "OP_SETVARLF",
			    "OP_SETOUTVAR",
  };
  if(type < NODE_MAX)
    return str[type];
  else
    return opstr[type-NODE_MAX];
 fail:
  return 0;
}

ts_tree_node_t *ts_tree_node_new(ts_tree_node_type_t type, int l, int c)
{
  ts_tree_node_t *tn = calloc(1, sizeof(ts_tree_node_t));
  TS_TRY(tn);
  tn->type = type;
  tn->nchild = 0;
  tn->child_ar = 0;
  tn->l = l;
  tn->c = c;
  return tn;
 fail:
  return 0;  
}

ts_tree_node_t *ts_tree_node_new_end_int(ts_tree_node_type_t type, int value, int l, int c)
{
  ts_tree_node_t *tn = calloc(1, sizeof(ts_tree_node_t));
  TS_TRY(tn);
  tn->type = type;
  tn->valuei = value;
  tn->nchild = 0;
  tn->child_ar = 0;
  tn->l = l;
  tn->c = c;
  return tn;
 fail:
  return 0;
}

ts_tree_node_t *ts_tree_node_new_end(ts_tree_node_type_t type, char *value, int l, int c)
{
  ts_tree_node_t *tn = calloc(1, sizeof(ts_tree_node_t));
  TS_TRY(tn);
  tn->type = type;
  TS_TRY(tn->value = strdup(value));
  tn->nchild = 0;
  tn->child_ar = 0;
  tn->l = l;
  tn->c = c;
  return tn;
 fail:
  return 0;
}

ts_tree_node_t *ts_tree_node_new1(ts_tree_node_type_t type, ts_tree_node_t *child1, int l, int c)
{
  TS_TRY(type && child1);
  ts_tree_node_t *tn = calloc(1, sizeof(ts_tree_node_t));
  TS_TRY(tn);
  tn->type = type;
  tn->nchild = 1;
  TS_TRY(tn->child_ar = calloc(1, sizeof(ts_tree_node_t*)));
  tn->child_ar[0] = child1;
  tn->l = l;
  tn->c = c;
  return tn;
 fail:
  return 0;
}

ts_tree_node_t *ts_tree_node_new2(ts_tree_node_type_t type, ts_tree_node_t *child1, ts_tree_node_t *child2, int l, int c)
{
  TS_TRY(type && child1 && child2);
  ts_tree_node_t *tn = calloc(1, sizeof(ts_tree_node_t));
  TS_TRY(tn);
  tn->type = type;
  tn->nchild = 2;
  TS_TRY(tn->child_ar = calloc(2, sizeof(ts_tree_node_t*)));
  tn->child_ar[0] = child1;
  tn->child_ar[1] = child2;
  tn->l = l;
  tn->c = c;
  return tn;
 fail:
  return 0;
}

ts_tree_node_t *ts_tree_node_new3(ts_tree_node_type_t type, ts_tree_node_t *child1, ts_tree_node_t *child2, ts_tree_node_t *child3, int l, int c)
{
  TS_TRY(type && child1 && child2 && child3);
  ts_tree_node_t *tn = calloc(1, sizeof(ts_tree_node_t));
  TS_TRY(tn);
  tn->type = type;
  tn->nchild = 3;
  TS_TRY(tn->child_ar = calloc(3, sizeof(ts_tree_node_t*)));
  tn->child_ar[0] = child1;
  tn->child_ar[1] = child2;
  tn->child_ar[2] = child3;
  tn->l = l;
  tn->c = c;
  return tn;
 fail:
  return 0;
}

int ts_tree_node_add_child(ts_tree_node_t *tn, ts_tree_node_t *child)
{
  TS_TRY(tn && child);
  tn->nchild ++;
  tn->child_ar = realloc(tn->child_ar, tn->nchild * sizeof(ts_tree_node_t*));
  tn->child_ar[tn->nchild-1] = child;
  return 1;
 fail:
  return 0;
}

int ts_tree_node_print(ts_tree_node_t *tn, int level)
{
  TS_TRY(tn);
  printf("%*s(%s)",4*level, "", nodetype_to_str(tn->type));
  switch(tn->type) {
  case NODE_INT:case NODE_FLOAT:case NODE_DOUBLE:case NODE_STR:case NODE_VAR:case NODE_TIME_CONST:
    printf("%s", tn->value);
    break;
  case NODE_FUNCTION:
    printf("(%d)", tn->valuei);
  }
  printf("[%d|%d]\n", tn->l, tn->c);
  int i=0;
  for(i=0;i<tn->nchild;i++) {
    ts_tree_node_print(tn->child_ar[i], level+1);
  }
  return 1;
 fail:
  return 0;
}

int ts_tree_node_free(ts_tree_node_t *tn)
{
  TS_TRY(tn);
  ts_free(tn->value);
  int i=0;
  for(i=0;i<tn->nchild;i++) {
    ts_tree_node_free(tn->child_ar[i]);
    tn->child_ar[i] = 0;
  }
  ts_free(tn->value);
  ts_free(tn->child_ar);
  ts_free(tn);
  return 1;
 fail:
  return 0;
}

int ts_tree_node_eq(ts_tree_node_t *t1, ts_tree_node_t *t2)
{ // return 0 means eq
  assert(t1 && t2);
  if(t1->type != t2->type || t1->nchild != t2->nchild ||
     (t1->value && t2->value && (0 != strcmp(t1->value, t2->value))) ||
     t1->valuei != t2->valuei) {
    return 1;  
  }
  int i;
  for(i=0;i<t1->nchild;i++) {
    if(1 == ts_tree_node_eq(t1->child_ar[i], t2->child_ar[i])) {
      return 1;
    }
  }
  return 0; //finally they are eq
}
