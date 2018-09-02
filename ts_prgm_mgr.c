#include "ts_prgm_mgr.h"
#include "ts_parse.h"
#include "ts_vm.h"
#include <stdlib.h>
#include <stdio.h>

extern ts_vm_t *vm;
extern int ts_linenumber;
extern int ts_colnumber;
static int _ts_prgm_mgr_append_prgm(ts_prgm_mgr_t *pm, ts_prgm_t *p);
static int ts_prgm_mgr_process_main_tree(ts_prgm_mgr_t *pm, ts_tree_node_t *t);
static int ts_prgm_mgr_process_lib_tree(ts_prgm_mgr_t *pm, ts_tree_node_t *t, short *res_pid);

ts_prgm_mgr_t *ts_prgm_mgr_new()
{
  ts_prgm_mgr_t *pm = calloc(1, sizeof(ts_prgm_mgr_t));
  TS_TRY(pm);
  
  pm->mp = 10;
  pm->np = 0;
  TS_TRY(pm->prgms_ar = calloc(10, sizeof(ts_prgm_t*)));
  TS_TRY(pm->pt = ts_hash_table_new(1000, sizeof(int)));
  return pm;
 fail:
  return 0;
}

int ts_prgm_mgr_free(ts_prgm_mgr_t *pm)
{
  TS_TRY(pm);
  if(pm->before) {
    ts_prgm_free(pm->before);
  }
  if(pm->after) {
    ts_prgm_free(pm->after);
  }
  int i;
  for(i=0;i<pm->np;i++) {
    ts_prgm_free(pm->prgms_ar[i]);
  }
  free(pm);
  return 1;
 fail:
  return 0;
}

int _ts_prgm_mgr_append_prgm(ts_prgm_mgr_t *pm, ts_prgm_t *p)
{
  TS_TRY(pm && p);

  if(pm->np == pm->mp) {
    ts_realloc(pm->prgms_ar, pm->mp, 10, ts_prgm_t*);
    pm->mp += 10;
  }
  pm->prgms_ar[pm->np] = p;
  TS_TRY(ts_hash_table_insert(pm->pt, p->name, &pm->np));
  pm->np++;

  return 1;
 fail:
  return 0;
}

int ts_prgm_mgr_process_lib_tree(ts_prgm_mgr_t *pm, ts_tree_node_t *t, short *res_pid)
{
  TS_TRY(pm && t && res_pid);
  ts_prgm_t *tp = ts_prgm_new();
  TS_TRY(tp);
  
  TS_TRY(t->nchild == 3);
  TS_TRY(ts_prgm_optimize(t->child_ar[1], pm->cur_formula->mfuncinstid, &tp->mfuncinstid));
  TS_TRY(ts_prgm_byte_code_generate(t->child_ar[1], tp));
  if(t->child_ar[1]->value) {
    tp->name = strdup(t->child_ar[1]->value);
  } else {
    (void)ts_warn(stderr, "runtime error @[%d|%d]: lib prgm can not be anonymouns.\n", vm->l, vm->c);
    goto fail;
  }

  *res_pid = pm->np;
  TS_TRY(_ts_prgm_mgr_append_prgm(pm, tp));

  return 1;
 fail:
  return 0;
}

int ts_prgm_mgr_process_main_tree(ts_prgm_mgr_t *pm, ts_tree_node_t *t)
{
  TS_TRY(pm && t);
  if(!pm->before) {
    TS_TRY(pm->before = ts_prgm_new());
  }
  if(!pm->after) {
    TS_TRY(pm->after = ts_prgm_new());
  }
  if(!pm->cur_formula) {
    TS_TRY(pm->cur_formula = ts_prgm_new());
  }

  TS_TRY(t->nchild == 3);
  TS_TRY(ts_prgm_optimize(t->child_ar[0], 0, &pm->before->mfuncinstid));
  TS_TRY(ts_prgm_optimize(t->child_ar[2], pm->before->mfuncinstid, &pm->after->mfuncinstid));
  TS_TRY(ts_prgm_optimize(t->child_ar[1], pm->after->mfuncinstid, &pm->cur_formula->mfuncinstid));
  
  TS_TRY(ts_prgm_byte_code_generate(t->child_ar[0], pm->before));
  TS_TRY(ts_prgm_byte_code_generate(t->child_ar[2], pm->after));
  TS_TRY(ts_prgm_byte_code_generate(t->child_ar[1], pm->cur_formula));  

  pm->before->name = strdup(".before");
  if(t->child_ar[1]->value) {
    pm->cur_formula->name = strdup(t->child_ar[1]->value);
  } else {
    pm->cur_formula->name = strdup("anonymous");
  }
  pm->after->name = strdup(".after");
  
  TS_TRY(_ts_prgm_mgr_append_prgm(pm, pm->cur_formula));
  
  return 1;
 fail:
  return 0;
}

#ifdef DEBUG
int ts_prgm_mgr_print(ts_prgm_mgr_t *pm)
{
  TS_TRY(pm);
  
  if(pm->before) {
    printf("-------------%s---------------------\n", pm->before->name);
    TS_TRY(ts_prgm_print(pm->before));
  }
  if(pm->cur_formula) {
    printf("-------------%s---------------------\n", pm->cur_formula->name);
    TS_TRY(ts_prgm_print(pm->cur_formula));
  }
  if(pm->after) {
    printf("-------------%s---------------------\n", pm->after->name);
    TS_TRY(ts_prgm_print(pm->after));
  }
  int i;
  for(i=0;i<pm->np;i++) {
    if(pm->prgms_ar[i] != pm->cur_formula) {
      printf("--------------%s---------------------\n", pm->prgms_ar[i]->name);
      TS_TRY(ts_prgm_print(pm->prgms_ar[i]));
    }
  }
  
  return 1;
 fail:
  return 0;
}
#endif

int ts_prgm_mgr_clear(ts_prgm_mgr_t *pm)
{
  TS_TRY(pm);
  TS_TRY(ts_prgm_clear(pm->before));
  TS_TRY(ts_prgm_clear(pm->after));
  pm->cur_formula = 0;
  return 1;
 fail:
  return 0;
}

int ts_prgm_mgr_load_main(ts_prgm_mgr_t *pm, const char *path, int isstdin)
{
  TS_TRY(pm);
  char buf[BUFSIZ];
  FILE *fp = 0;
  ts_tree_node_t *synt_tree;

  ts_linenumber = 0;
  ts_colnumber = 0;

  if(!isstdin) {
    strcpy(buf, path);
    char *end = strrchr(buf, '/');
    if(end) {
      *(end+1) = '\0';
    }
    pm->basedir = strdup(buf);
    fp = fopen(path, "r");
    if(!fp) {
      (void)ts_warn(stderr, "runtime error @[%d|%d]: can not open [%s].\n", vm->l, vm->c, path);
      goto fail;
    }
    ts_parsef(fp, &synt_tree);
    fclose(fp);
  } else {
    ts_parsef(stdin, &synt_tree);
  }
 
  TS_TRY(ts_prgm_mgr_process_main_tree(pm, synt_tree));
#ifdef DEBUG
  TS_TRY(ts_tree_node_print(synt_tree, 0));
#endif
  TS_TRY(ts_tree_node_free(synt_tree));

#ifdef DEBUG
  TS_TRY(ts_prgm_mgr_print(pm));
#endif

  return 1;
 fail:
  return 0;
}

int ts_prgm_mgr_load_lib(ts_prgm_mgr_t *pm, const char *pname, short *res_pid)
{
  TS_TRY(pm && pname && res_pid);
  char buf[BUFSIZ];
  FILE *fp=0;
  ts_tree_node_t *synt_tree;
  short pid;

  ts_linenumber = 0;
  ts_colnumber = 0;

  snprintf(buf, BUFSIZ, "%s/%s.t", pm->basedir, pname);
  fp = fopen(buf, "r");
  if(!fp) {
    (void)ts_warn(stderr, "runtime error @[%d|%d]: formula [%s] can not found.\n", vm->l, vm->c, buf);
    goto fail;
  }
  ts_parsef(fp, &synt_tree);
  fclose(fp);
  TS_TRY(ts_prgm_mgr_process_lib_tree(pm, synt_tree, &pid));
#ifdef DEBUG
  TS_TRY(ts_tree_node_print(synt_tree, 0));
#endif
  TS_TRY(ts_tree_node_free(synt_tree));
#ifdef DEBUG
  TS_TRY(ts_prgm_print(pm->prgms_ar[pid]));
#endif

  *res_pid = pid;
  return 1;
 fail:
  return 0;
}
