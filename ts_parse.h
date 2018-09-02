#ifndef TS_PARSE_H
#define TS_PARSE_H

#include "ts_tree.h"

typedef struct parse_parm
{
  int mode;
  FILE *fp;
  void *yyscanner;
  char *buf;
  int pos;
  int length;
  ts_tree_node_t *result;
} parse_parm_t;

void ts_parsestr(char *buf, ts_tree_node_t **result);
void ts_parsef(FILE *f, ts_tree_node_t **result);

#endif
