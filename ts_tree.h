#ifndef TS_TREE_H
#define TS_TREE_H

#include "ts_misc.h"
#include "ts_type.h"

typedef enum ts_tree_node_type {
  NODE_PRGM = 1,
  NODE_FORMULA,
  NODE_STMTS,
  NODE_IFELSE,
  NODE_ADD,
  NODE_MINUS,
  NODE_MUL,
  NODE_DIV,
  NODE_MOD,
  NODE_GT,
  NODE_LT,
  NODE_EQ,
  NODE_NEQ,
  NODE_LE,
  NODE_GE,
  NODE_AND,
  NODE_OR,
  NODE_DOT,
  NODE_NOT,
  NODE_NEGATIVE,
  NODE_OUTPUT_ASSIGN,
  NODE_ASSIGN,
  NODE_NEWOUTVAR,
  NODE_NEWVAR,
  NODE_FUNCTION,
  NODE_PARA_LIST,
  NODE_STR,
  NODE_VAR,
  NODE_INT,
  NODE_FLOAT,
  NODE_DOUBLE,
  NODE_TIME_CONST,
  NODE_EMPTY,
  NODE_MAX,
} ts_tree_node_type_t;

typedef struct ts_tree_node {
  int type;
  int nchild;
  char *value;
  int valuei;
  struct ts_tree_node **child_ar;
  int l;
  int c;
} ts_tree_node_t;

ts_tree_node_t *ts_tree_node_new(ts_tree_node_type_t type, int l, int c);
ts_tree_node_t *ts_tree_node_new_end(ts_tree_node_type_t type, char *value, int l, int c);
ts_tree_node_t *ts_tree_node_new_end_int(ts_tree_node_type_t type, int value, int l, int c);
ts_tree_node_t *ts_tree_node_new1(ts_tree_node_type_t type, ts_tree_node_t *child1, int l, int c);
ts_tree_node_t *ts_tree_node_new2(ts_tree_node_type_t type, ts_tree_node_t *child1, ts_tree_node_t *child2, int l, int c);
ts_tree_node_t *ts_tree_node_new3(ts_tree_node_type_t type, ts_tree_node_t *child1, ts_tree_node_t *child2, ts_tree_node_t *child3, int l, int c);

int ts_tree_node_add_child(ts_tree_node_t * tn, ts_tree_node_t *child);
int ts_tree_node_print(ts_tree_node_t *tn, int level);
char* nodetype_to_str(int type);
int ts_tree_node_free(ts_tree_node_t *tn);
int ts_tree_node_eq(ts_tree_node_t *t1, ts_tree_node_t *t2);

#endif
