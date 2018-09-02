#ifndef TS_PRGM_H
#define TS_PRGM_H

#include "ts_tree.h"

typedef enum {
  OP_NOP = NODE_MAX,
  OP_PUSHG, //generic
  OP_PUSH, // push value ptr
  OP_POP, // pop value ptr
  OP_JMP,
  OP_JMPF, //JMP if false on the top of stack, generic type (var(contains int) or int allowed), and pop top
  OP_ADD,
  OP_MINUS,
  OP_MUL,
  OP_DIV,
  OP_MOD,
  OP_AND,
  OP_OR,
  OP_LT,
  OP_GT,
  OP_EQ,
  OP_NEQ,
  OP_LE,
  OP_GE,
  OP_DOT,
  OP_DOTI,
  OP_DOTT,
  OP_NOT,
  OP_NEG,
  OP_CALL,
  OP_CALLI,
  OP_SETVAR,
  OP_SETSVAR,
  OP_SETVARI,
  OP_SETVARF,
  OP_SETVARLF,
  OP_SETOUTVAR,
  OP_MAX,
} opcode_t;

typedef struct instruction_oprand {
  ts_type_t type;
  union {
    int i;
    float f;
    double lf;
    char* s;
    ts_time_t t;
    struct {
      short tid;
      union {
	short fid;
	short pid;
	short vid;
      };
    };
    struct {
      char *tablename;
      char *varname;
    };
  };
} ts_stack_elem_t;


typedef struct instruction {
  opcode_t opcode;
#ifdef DEBUG
  int ln;
#endif
  int l;
  int c;
  union {
    ts_stack_elem_t oprand;
    struct {
      ts_type_t type;
      union {
	int i;
	float f;
	double lf;
	char* s;
	ts_time_t t;
	struct {
	  short tid;
	  union {
	    short fid;
	    short pid;
	    short vid;
	  };
	};
	struct {
	  char *tablename;
	  char *varname;
	};
      };
    };
  };
} instruction_t;



typedef struct ts_prgm {
  char *name;
  int mins;
  int nins;
  instruction_t *prgm_ar;
  int mfuncinstid;

  // str for gc
  int ms;
  int ns;
  char **str_ar;

  // svar args
  int argc;
  char **argvname;
  double *argv;
  short *svarid_ar;

  int mvar;
  int nvar;
  int *varid_ar; //vars used in the prgm
} ts_prgm_t;

ts_prgm_t *ts_prgm_new(void);
int ts_prgm_clear(ts_prgm_t *prgm);
int ts_prgm_free(ts_prgm_t *prgm);
int ts_prgm_append(ts_prgm_t *prgm, opcode_t opcode, ts_type_t type, void *oprand1, void *oprand2, int l, int c);
int ts_prgm_optimize(ts_tree_node_t *tn, int funcinstid_start, int *mfuncinstid);
int ts_prgm_byte_code_generate(ts_tree_node_t *tn, ts_prgm_t *prgm);
int ts_prgm_gen_arglist(ts_prgm_t *prgm, ts_tree_node_t *pl);
int ts_prgm_append_new_varid(ts_prgm_t *p, int vid);

#ifdef DEBUG
int ts_prgm_print(ts_prgm_t *prgm);
#endif
#endif
