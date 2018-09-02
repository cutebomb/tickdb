%{
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include "ts_type.h"
#include "ts_vm.h"
#include "ts_tree.h"
#include "ts_hash_table.h"
#include "ts_parse.h"

#define YYDEBUG 1

void yyerror (parse_parm_t *pp, void *scanner, char *s);
int valid_digits(const char *v);
extern int ts_linenumber;
extern int ts_colnumber;
extern ts_vm_t *vm;
static int ivarcount = 0;
%}

%pure_parser
%parse-param {parse_parm_t *parm}
%parse-param {void *scanner}
%lex-param {yyscan_t *scanner}

%union
{
  char *value;
  void *Tree_node;
}

%start program

%token ERROR_TOKEN ASSIGN OUTPUT_ASSIGN INT_NUM NUM ID TIME_CONST QUOTESTR SEMICOLON OPARA CPARA COMMA OBRACE CBRACE BQUOTE SHARP IF ELSE

%right OUTPUT_ASSIGN ASSIGN
%left AND OR
%left NEQ EQ LT GT LE GE 
%left ADD MINUS
%left MUL DIV MOD
%left DOT
%left NOT

%type <Tree_node> program stmts stmt formula expr para_list lvalue QUOTESTR ID NUM INT_NUM TIME_CONST

%%
program : stmts formula stmts
        {
	  ts_tree_node_t *synt_tree = ts_tree_node_new(NODE_PRGM, ts_linenumber, ts_colnumber);
	  ts_tree_node_add_child(synt_tree, $1);
	  ts_tree_node_add_child(synt_tree, $2);
	  ts_tree_node_add_child(synt_tree, $3);
	  parm->result = synt_tree;
        };

formula: OBRACE stmts CBRACE
       {
	 ts_tree_node_t *formula = ts_tree_node_new(NODE_FORMULA, ts_linenumber, ts_colnumber);
	 ts_tree_node_t *paralist = ts_tree_node_new(NODE_PARA_LIST, ts_linenumber, ts_colnumber);
	 ts_tree_node_add_child(formula, paralist);
	 ts_tree_node_add_child(formula, $2);
	 $$ = formula;
       }
       | ID OBRACE stmts CBRACE
       {
	 ts_tree_node_t *formula = ts_tree_node_new_end(NODE_FORMULA, $1, ts_linenumber, ts_colnumber);
	 ts_tree_node_t *paralist = ts_tree_node_new(NODE_PARA_LIST, ts_linenumber, ts_colnumber);
	 ts_tree_node_add_child(formula, paralist);
	 ts_tree_node_add_child(formula, $3);
	 $$ = formula;
       }
       | ID OPARA para_list CPARA OBRACE stmts CBRACE
       {
	 ts_tree_node_t *formula = ts_tree_node_new_end(NODE_FORMULA, $1, ts_linenumber, ts_colnumber);
	 ts_tree_node_add_child(formula, $3);
	 ts_tree_node_add_child(formula, $6);
	 $$ = formula;
       }
     | {$$ = ts_tree_node_new(NODE_FORMULA, ts_linenumber, ts_colnumber);};

stmts : stmts stmt
     {
       ts_tree_node_add_child($1, $2);
       $$ = $1;
     }
     |
     {
       $$ = ts_tree_node_new(NODE_STMTS, ts_linenumber, ts_colnumber);
     }
     ;
stmt : expr SEMICOLON { $$ = $1; }
     | expr {$$ = $1;}
     ;
expr : OPARA expr CPARA { $$ = $2; }
     | IF OPARA expr CPARA OBRACE stmts CBRACE
     {
       ts_tree_node_t *empty = ts_tree_node_new(NODE_EMPTY, ts_linenumber, ts_colnumber);
       $$ = ts_tree_node_new3(NODE_IFELSE, $3, $6, empty, ts_linenumber, ts_colnumber);
     }
     | IF OPARA expr CPARA OBRACE stmts CBRACE ELSE OBRACE stmts CBRACE
     {
       $$ = ts_tree_node_new3(NODE_IFELSE, $3, $6, $10, ts_linenumber, ts_colnumber);
     }
     | expr ADD expr
     {
       $$ = ts_tree_node_new2(NODE_ADD, $1, $3, ts_linenumber, ts_colnumber);
     }
     | expr MINUS expr 
     {
       $$ = ts_tree_node_new2(NODE_MINUS, $1, $3, ts_linenumber, ts_colnumber);
     }
     | expr MUL expr
     {
       $$ = ts_tree_node_new2(NODE_MUL, $1, $3, ts_linenumber, ts_colnumber);
     }
     | expr DIV expr
     {
       $$ = ts_tree_node_new2(NODE_DIV, $1, $3, ts_linenumber, ts_colnumber);
     }
     | expr MOD expr
     {
       $$ = ts_tree_node_new2(NODE_MOD, $1, $3, ts_linenumber, ts_colnumber);
     }
     | expr GT expr { $$ = ts_tree_node_new2(NODE_GT, $1, $3, ts_linenumber, ts_colnumber); }
     | expr LT expr { $$ = ts_tree_node_new2(NODE_LT, $1, $3, ts_linenumber, ts_colnumber); }
     | expr EQ expr { $$ = ts_tree_node_new2(NODE_EQ, $1, $3, ts_linenumber, ts_colnumber); }
     | expr NEQ expr { $$ = ts_tree_node_new2(NODE_NEQ, $1, $3, ts_linenumber, ts_colnumber); }
     | expr LE expr { $$ = ts_tree_node_new2(NODE_LE, $1, $3, ts_linenumber, ts_colnumber); }
     | expr GE expr { $$ = ts_tree_node_new2(NODE_GE, $1, $3, ts_linenumber, ts_colnumber); }
     | expr AND expr { $$ = ts_tree_node_new2(NODE_AND, $1, $3, ts_linenumber, ts_colnumber); }
     | expr OR expr { $$ = ts_tree_node_new2(NODE_OR, $1, $3, ts_linenumber, ts_colnumber); }
     | NOT expr { $$ = ts_tree_node_new1(NODE_NOT, $2, ts_linenumber, ts_colnumber); }
     | MINUS expr { $$ = ts_tree_node_new1(NODE_NEGATIVE, $2, ts_linenumber, ts_colnumber);}
     | BQUOTE ID OPARA para_list CPARA DOT ID SHARP ID BQUOTE
     {
       ts_tree_node_t *fname = ts_tree_node_new_end(NODE_VAR, ".getOther", ts_linenumber, ts_colnumber);
       ts_tree_node_t *pl = ts_tree_node_new(NODE_PARA_LIST, ts_linenumber, ts_colnumber);
       ts_tree_node_t *tableName = ts_tree_node_new_end(NODE_STR, $9, ts_linenumber, ts_colnumber);
       ts_tree_node_t *otherFormulaName = ts_tree_node_new_end(NODE_STR, $2, ts_linenumber, ts_colnumber);
       ts_tree_node_t *otherVarName = ts_tree_node_new_end(NODE_STR, $7, ts_linenumber, ts_colnumber);
       ts_tree_node_add_child(pl, tableName);
       ts_tree_node_add_child(pl, otherFormulaName);
       ts_tree_node_add_child(pl, otherVarName);
       int i,nc = ((ts_tree_node_t*)$4)->nchild;
       for(i=0;i<nc;i++) {
	 ts_tree_node_add_child(pl, ((ts_tree_node_t*)$4)->child_ar[i]);
       }
       $$ = ts_tree_node_new2(NODE_FUNCTION, fname, pl, ts_linenumber, ts_colnumber);
       free($2);
       free($7);
       free($9);
     }
     | lvalue { $$ = $1;}
     | lvalue OUTPUT_ASSIGN expr
     { 
       $$ = ts_tree_node_new2(NODE_OUTPUT_ASSIGN, $1, $3, ts_linenumber, ts_colnumber);
     }
     | lvalue ASSIGN expr
     {
       $$ = ts_tree_node_new2(NODE_ASSIGN, $1, $3, ts_linenumber, ts_colnumber); 
     }
     | ID OPARA para_list CPARA
     {
       ts_tree_node_t *t1, *t2, *t3, *stmts, *iv;
       short funcid;
       ts_func_mgr_get_funcid(vm->func_mgr, $1, &funcid);
       if(ts_func_mgr_is_sfunc(vm->func_mgr, funcid)) {
	 int nsvar, i;
	 int *psvaridx_ar;
	 ts_func_mgr_get_sfunc_svar_ar(vm->func_mgr, funcid, &nsvar, &psvaridx_ar);
	 t1 = $3;
	 for(i=0;i<nsvar;i++) {
	   if(t1->child_ar[psvaridx_ar[i]]->type != NODE_VAR) {
	     char buf[100];
	     snprintf(buf, 100, ".i%d", ivarcount++);
	     t3 = ts_tree_node_new_end(NODE_VAR, buf, ts_linenumber, ts_colnumber);
	     t2 = ts_tree_node_new2(NODE_ASSIGN, t3, t1->child_ar[psvaridx_ar[i]], ts_linenumber, ts_colnumber);
	     stmts = ts_tree_node_new(NODE_STMTS, ts_linenumber, ts_colnumber);
	     iv = ts_tree_node_new_end(NODE_VAR, buf, ts_linenumber, ts_colnumber);
	     ts_tree_node_add_child(stmts, t2);
	     ts_tree_node_add_child(stmts, iv);
	     t1->child_ar[psvaridx_ar[i]] = stmts;
	   }
	 }
       }
       ts_tree_node_t *tn = ts_tree_node_new_end(NODE_VAR, $1, ts_linenumber, ts_colnumber);
       $$ = ts_tree_node_new2(NODE_FUNCTION, tn, $3, ts_linenumber, ts_colnumber);
       free($1);
     }
     | QUOTESTR { $$ = ts_tree_node_new_end(NODE_STR, $1, ts_linenumber, ts_colnumber); free($1); }
     | ID { $$ = ts_tree_node_new_end(NODE_VAR, $1, ts_linenumber, ts_colnumber); free($1); }
     | NUM {
         int end = strlen($1)-1;
	 if(((char*)$1)[end] == 'f') {
	   if(((char*)$1)[end-1] == 'l') {
	     $$ = ts_tree_node_new_end(NODE_DOUBLE, $1, ts_linenumber, ts_colnumber);
	   } else {
	     $$ = ts_tree_node_new_end(NODE_FLOAT, $1, ts_linenumber, ts_colnumber);
	   }
	 } else {
	   int n = valid_digits($1);
	   if(n > 7) {
	     $$ = ts_tree_node_new_end(NODE_DOUBLE, $1, ts_linenumber, ts_colnumber);
	   } else {
	     $$ = ts_tree_node_new_end(NODE_FLOAT, $1, ts_linenumber, ts_colnumber);
	   }
	 }
	 free($1);
       }
     | INT_NUM { $$ = ts_tree_node_new_end(NODE_INT, $1, ts_linenumber, ts_colnumber);free($1);}
     | TIME_CONST {$$ = ts_tree_node_new_end(NODE_TIME_CONST, $1, ts_linenumber, ts_colnumber);free($1);}
     ;

para_list : para_list COMMA expr { ts_tree_node_add_child($1, $3); $$ = $1; }
          | expr { $$ = ts_tree_node_new1(NODE_PARA_LIST, $1, ts_linenumber, ts_colnumber); }
          | {$$ = ts_tree_node_new(NODE_PARA_LIST, ts_linenumber, ts_colnumber);}
	  ;

lvalue : ID DOT ID
     {
       ts_tree_node_t *t1 = ts_tree_node_new_end(NODE_VAR, $1, ts_linenumber, ts_colnumber);
       ts_tree_node_t *t2 = ts_tree_node_new_end(NODE_VAR, $3, ts_linenumber, ts_colnumber);
       $$ = ts_tree_node_new2(NODE_DOT, t1, t2, ts_linenumber, ts_colnumber);
       free($1);
       free($3);
     }
     | ID  { $$ = ts_tree_node_new_end(NODE_VAR, $1, ts_linenumber, ts_colnumber); free($1); }

%%

void yyerror (parse_parm_t *pp, void *scanner, char *s)
{
  printf("%s at %d:%d\n", s, ts_linenumber, ts_colnumber);
}

int valid_digits(const char *v)
{
  int n=0;
  while(*v) {
    if(isdigit(*v)) {
      v++;
      n++;
    }
    else if(*v == '.') {
      v++;
      continue;
    } else {
      break;
    }
  }
  return n;
}

