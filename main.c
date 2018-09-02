#include <stdio.h>
#include <float.h>
#include "ts_tree.h"
#include "ts_vm.h"
#include "ts_prgm_mgr.h"

extern int ts_linenumber;
extern int ts_colnumber;
ts_vm_t *vm = 0;

void usage(const char *p)
{
}

int main(int argc, char *argv[])
{
  ts_prgm_mgr_t *pm = 0;
  TS_TRY(pm = ts_prgm_mgr_new());
  TS_TRY(vm = ts_vm_new(pm));
  if(argc == 2) {
    TS_TRY(ts_prgm_mgr_load_main(pm, argv[1], 0));
    TS_TRY(ts_vm_run_once(vm, pm->before));
    TS_TRY(ts_vm_run(vm, pm->cur_formula, VM_MODE_MAIN));
    TS_TRY(ts_vm_run_once(vm, pm->after));
    TS_TRY(ts_prgm_mgr_clear(pm));
  } else {
    while(1) {
      ts_linenumber = 1;
      ts_colnumber = 1;
      TS_TRY(ts_prgm_mgr_load_main(pm, 0, 1));
      TS_TRY(ts_vm_run_once(vm, pm->before));
      TS_TRY(ts_vm_run(vm, pm->cur_formula, VM_MODE_MAIN));
      TS_TRY(ts_vm_run_once(vm, pm->after));
      TS_TRY(ts_prgm_mgr_clear(pm));
    }
  }
  TS_TRY(ts_prgm_mgr_free(pm));
  TS_TRY(ts_vm_free(vm));
  return 0;
 fail:
  return 1;
}
