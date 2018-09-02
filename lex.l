%{
#include "parse.h"
#include "ts_tree.h"
#include "ts_parse.h"
#include <stdio.h>
#include <string.h>

void yyparse(parse_parm_t *pp, void *scanner);
int ts_linenumber = 1;
int ts_colnumber = 1;

/*
** When in the lexer you have to access parm through the extra data.
*/

#define YY_EXTRA_TYPE parse_parm_t*

/*
** We want to read from a the buffer in parm so we have to redefine the
** YY_INPUT macro (see section 10 of the flex manual 'The generated scanner')
*/
#define YY_NO_INPUT

#define YY_INPUT(buffer, res, max_size)				\
do {								\
  if(yyextra->mode == 0) {                                      \
    if (yyextra->pos >= yyextra->length)			\
		res = YY_NULL;					\
	else							\
	{							\
		res = yyextra->length - yyextra->pos;		\
		res > (int)max_size ? res = max_size : 0;	\
		memcpy(buffer, yyextra->buf + yyextra->pos, res);\
		yyextra->pos += res;				\
	}							\
  } else {                                                      \
    char c;                                                     \
    c = fgetc(yyextra->fp);                                     \
    if(feof(yyextra->fp))                                       \
      res = YY_NULL;                                            \
    else {                                                      \
      buffer[0] = c;                                            \
      res = 1;                                                  \
    }                                                           \
  }                                                             \
} while (0)

%}

/*
** We want the scanner to be reentrant, therefore generate no global variables.
** That what the 'reentrant' option is for.
** 'bison-bridge' is used to create a bison compatible scanner and share yylval
*/
%option reentrant bison-bridge
%option noyywrap
%option nounput

digit           [0-9]
and             (and)|(AND)|(&&)
letter          [A-Za-z]
id              ({letter}|[_])({letter}|{digit}|[_])*
int_num         {digit}+
num	        {int_num}([.]{int_num}?)?([eE][+|-]?{int_num})?("lf"|"f")?
wspace          [ \t]+
qs              [\'][^\']*[\']
timeconst       ({num}[dhms])+

%%

{timeconst}  {ts_colnumber += strlen(yytext);yylval->value = strdup(yytext);return TIME_CONST;}
{int_num} {
          yylval->value = strdup(yytext);
          ts_colnumber += strlen(yytext);
          return INT_NUM;
         }
{num}	{
          yylval->value = strdup(yytext);
          ts_colnumber += strlen(yytext);
          return NUM;
        }
("and")|("AND")|("&&")  {ts_colnumber += strlen(yytext);return AND;}
("or")|("OR")|("||")    {ts_colnumber += strlen(yytext);return OR;}
("NOT")|("not")|("!")   {ts_colnumber += strlen(yytext);return NOT;}
"if"    { ts_colnumber += 2;return IF; }
"else"  { ts_colnumber += 4;return ELSE; }
{qs}    { 
          ts_colnumber += strlen(yytext);
          yylval->value = strdup(yytext+1);
          yylval->value[strlen(yylval->value)-1] = '\0';
          return QUOTESTR;}
{id}	{
          yylval->value = strdup(yytext);
          ts_colnumber += strlen(yytext);
          return ID;
        }
"!="    {ts_colnumber += 2;return NEQ;}
"=="    {ts_colnumber += 2;return EQ;}
"="     {ts_colnumber += 1;return ASSIGN;}
":="    {ts_colnumber += 2;return OUTPUT_ASSIGN;}
"<"     {ts_colnumber += 1;return LT;}
">"     {ts_colnumber += 1;return GT;}
"<="    {ts_colnumber += 2;return LE;}
">="    {ts_colnumber += 2;return GE;}
"("     {ts_colnumber += 1;return OPARA;}
")"     {ts_colnumber += 1;return CPARA;}
"{"     {ts_colnumber += 1;return OBRACE;}
"}"     {ts_colnumber += 1;return CBRACE;}
","     {ts_colnumber += 1;return COMMA;}
";"     {ts_colnumber += 1;return SEMICOLON;}
"+"     {ts_colnumber += 1;return ADD;}
"-"     {ts_colnumber += 1;return MINUS;}
"*"     {ts_colnumber += 1;return MUL;}
"/"     {ts_colnumber += 1;return DIV;}
"%"     {ts_colnumber += 1;return MOD;}
"\n"    {ts_linenumber++;ts_colnumber = 1;}
{wspace}  {ts_colnumber += strlen(yytext);}
"."     {ts_colnumber += 1;return DOT;}
"`"     {ts_colnumber += 1;return BQUOTE;}
"#"     {ts_colnumber += 1;return SHARP;}
.	{printf("unexpect token %s\n", yytext);return ERROR_TOKEN;}

%%

void	ts_parsestr(char *buf, ts_tree_node_t **result)
{
	parse_parm_t	pp;

        pp.mode = 0;
	pp.buf = buf;
	pp.length = strlen(buf);
	pp.pos = 0;
	*result = 0;
	yylex_init(&pp.yyscanner);
	yyset_extra(&pp, pp.yyscanner);
	yyparse(&pp, pp.yyscanner);
	*result = pp.result;
	yylex_destroy(pp.yyscanner);
}

void ts_parsef(FILE *f, ts_tree_node_t **result)
{
  parse_parm_t pp;

  pp.mode = 1;
  pp.fp = f;
  *result = 0;
  
  yylex_init(&pp.yyscanner);
  yyset_extra(&pp, pp.yyscanner);
  yyparse(&pp, pp.yyscanner);
  *result = pp.result;
  yylex_destroy(pp.yyscanner);
}
