
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "parse.y"

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


/* Line 189 of yacc.c  */
#line 94 "parse.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ERROR_TOKEN = 258,
     ASSIGN = 259,
     OUTPUT_ASSIGN = 260,
     INT_NUM = 261,
     NUM = 262,
     ID = 263,
     TIME_CONST = 264,
     QUOTESTR = 265,
     SEMICOLON = 266,
     OPARA = 267,
     CPARA = 268,
     COMMA = 269,
     OBRACE = 270,
     CBRACE = 271,
     BQUOTE = 272,
     SHARP = 273,
     IF = 274,
     ELSE = 275,
     OR = 276,
     AND = 277,
     GE = 278,
     LE = 279,
     GT = 280,
     LT = 281,
     EQ = 282,
     NEQ = 283,
     MINUS = 284,
     ADD = 285,
     MOD = 286,
     DIV = 287,
     MUL = 288,
     DOT = 289,
     NOT = 290
   };
#endif
/* Tokens.  */
#define ERROR_TOKEN 258
#define ASSIGN 259
#define OUTPUT_ASSIGN 260
#define INT_NUM 261
#define NUM 262
#define ID 263
#define TIME_CONST 264
#define QUOTESTR 265
#define SEMICOLON 266
#define OPARA 267
#define CPARA 268
#define COMMA 269
#define OBRACE 270
#define CBRACE 271
#define BQUOTE 272
#define SHARP 273
#define IF 274
#define ELSE 275
#define OR 276
#define AND 277
#define GE 278
#define LE 279
#define GT 280
#define LT 281
#define EQ 282
#define NEQ 283
#define MINUS 284
#define ADD 285
#define MOD 286
#define DIV 287
#define MUL 288
#define DOT 289
#define NOT 290




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 27 "parse.y"

  char *value;
  void *Tree_node;



/* Line 214 of yacc.c  */
#line 207 "parse.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 219 "parse.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   280

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  36
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  8
/* YYNRULES -- Number of rules.  */
#define YYNRULES  43
/* YYNRULES -- Number of states.  */
#define YYNSTATES  94

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   290

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     7,    11,    16,    24,    25,    28,    29,
      32,    34,    38,    46,    58,    62,    66,    70,    74,    78,
      82,    86,    90,    94,    98,   102,   106,   110,   113,   116,
     127,   129,   133,   137,   142,   144,   146,   148,   150,   152,
     156,   158,   159,   163
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      37,     0,    -1,    39,    38,    39,    -1,    15,    39,    16,
      -1,     8,    15,    39,    16,    -1,     8,    12,    42,    13,
      15,    39,    16,    -1,    -1,    39,    40,    -1,    -1,    41,
      11,    -1,    41,    -1,    12,    41,    13,    -1,    19,    12,
      41,    13,    15,    39,    16,    -1,    19,    12,    41,    13,
      15,    39,    16,    20,    15,    39,    16,    -1,    41,    30,
      41,    -1,    41,    29,    41,    -1,    41,    33,    41,    -1,
      41,    32,    41,    -1,    41,    31,    41,    -1,    41,    25,
      41,    -1,    41,    26,    41,    -1,    41,    27,    41,    -1,
      41,    28,    41,    -1,    41,    24,    41,    -1,    41,    23,
      41,    -1,    41,    22,    41,    -1,    41,    21,    41,    -1,
      35,    41,    -1,    29,    41,    -1,    17,     8,    12,    42,
      13,    34,     8,    18,     8,    17,    -1,    43,    -1,    43,
       5,    41,    -1,    43,     4,    41,    -1,     8,    12,    42,
      13,    -1,    10,    -1,     8,    -1,     7,    -1,     6,    -1,
       9,    -1,    42,    14,    41,    -1,    41,    -1,    -1,     8,
      34,     8,    -1,     8,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    47,    47,    56,    64,    72,    79,    81,    87,    91,
      92,    94,    95,   100,   104,   108,   112,   116,   120,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     153,   154,   158,   162,   190,   191,   192,   210,   211,   214,
     215,   216,   219,   227
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ERROR_TOKEN", "ASSIGN", "OUTPUT_ASSIGN",
  "INT_NUM", "NUM", "ID", "TIME_CONST", "QUOTESTR", "SEMICOLON", "OPARA",
  "CPARA", "COMMA", "OBRACE", "CBRACE", "BQUOTE", "SHARP", "IF", "ELSE",
  "OR", "AND", "GE", "LE", "GT", "LT", "EQ", "NEQ", "MINUS", "ADD", "MOD",
  "DIV", "MUL", "DOT", "NOT", "$accept", "program", "formula", "stmts",
  "stmt", "expr", "para_list", "lvalue", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    36,    37,    38,    38,    38,    38,    39,    39,    40,
      40,    41,    41,    41,    41,    41,    41,    41,    41,    41,
      41,    41,    41,    41,    41,    41,    41,    41,    41,    41,
      41,    41,    41,    41,    41,    41,    41,    41,    41,    42,
      42,    42,    43,    43
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     3,     4,     7,     0,     2,     0,     2,
       1,     3,     7,    11,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,    10,
       1,     3,     3,     4,     1,     1,     1,     1,     1,     3,
       1,     0,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       8,     0,     6,     1,    37,    36,    35,    38,    34,     0,
       8,     0,     0,     0,     0,     8,     7,    10,    30,    41,
       8,     0,    35,     0,     0,     0,     0,    28,    27,     2,
       9,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,     0,     0,    42,
      41,    11,     3,    41,     0,    26,    25,    24,    23,    19,
      20,    21,    22,    15,    14,    18,    17,    16,    32,    31,
      33,     0,     4,     0,     0,     0,     8,    39,    33,     0,
       8,     0,     0,     0,     5,     0,    12,     0,     0,     0,
       8,    29,     0,    13
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,    15,     2,    16,    17,    47,    18
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -50
static const yytype_int16 yypact[] =
{
     -50,     3,    46,   -50,   -50,   -50,    35,   -50,   -50,   166,
     -50,     5,    -1,   166,   166,   -50,   -50,   181,     4,   166,
     -50,    15,    10,   202,    76,    33,   166,   -15,   -50,   166,
     -50,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   236,     7,    90,   -50,
     166,   -50,   -50,   166,   223,   247,   247,    83,    83,    83,
      83,    83,    83,   -15,   -15,   -50,   -50,   -50,   236,   236,
      34,   166,   -50,    28,    54,    36,   -50,   236,   -50,    14,
     -50,   114,    49,   128,   -50,    41,    40,    63,    47,    55,
     -50,   -50,   152,   -50
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -50,   -50,   -50,   -10,   -50,    -7,   -49,   -50
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -44
static const yytype_int8 yytable[] =
{
      24,    73,    23,     3,    74,    29,    27,    28,    44,    45,
      48,    26,    46,    25,   -43,   -43,    41,    42,    43,    54,
      70,    71,    50,    49,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,   -43,
     -43,    78,    71,    46,    21,    53,    46,    19,    82,    76,
      20,    80,     4,     5,     6,     7,     8,    85,     9,    87,
      88,    10,    90,    11,    77,    12,    81,    79,    71,    21,
      83,    89,    91,     0,     0,    13,     0,     0,     0,     0,
      92,    14,     4,     5,    22,     7,     8,     0,     9,     0,
       0,     0,    52,    11,     0,    12,     4,     5,    22,     7,
       8,     0,     9,     0,     0,    13,    72,    11,     0,    12,
       0,    14,    39,    40,    41,    42,    43,     0,     0,    13,
       4,     5,    22,     7,     8,    14,     9,     0,     0,     0,
      84,    11,     0,    12,     4,     5,    22,     7,     8,     0,
       9,     0,     0,    13,    86,    11,     0,    12,     0,    14,
       0,     0,     0,     0,     0,     0,     0,    13,     4,     5,
      22,     7,     8,    14,     9,     0,     0,     0,    93,    11,
       0,    12,     4,     5,    22,     7,     8,     0,     9,     0,
       0,    13,     0,    11,     0,    12,     0,    14,     0,     0,
       0,     0,    30,     0,     0,    13,     0,     0,     0,     0,
       0,    14,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    51,     0,     0,     0,     0,
       0,     0,     0,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    75,     0,     0,     0,
       0,     0,     0,     0,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43
};

static const yytype_int8 yycheck[] =
{
      10,    50,     9,     0,    53,    15,    13,    14,     4,     5,
      20,    12,    19,     8,     4,     5,    31,    32,    33,    26,
      13,    14,    12,     8,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,     4,
       5,    13,    14,    50,    34,    12,    53,    12,    34,    15,
      15,    15,     6,     7,     8,     9,    10,     8,    12,    18,
      20,    15,    15,    17,    71,    19,    76,    13,    14,    34,
      80,     8,    17,    -1,    -1,    29,    -1,    -1,    -1,    -1,
      90,    35,     6,     7,     8,     9,    10,    -1,    12,    -1,
      -1,    -1,    16,    17,    -1,    19,     6,     7,     8,     9,
      10,    -1,    12,    -1,    -1,    29,    16,    17,    -1,    19,
      -1,    35,    29,    30,    31,    32,    33,    -1,    -1,    29,
       6,     7,     8,     9,    10,    35,    12,    -1,    -1,    -1,
      16,    17,    -1,    19,     6,     7,     8,     9,    10,    -1,
      12,    -1,    -1,    29,    16,    17,    -1,    19,    -1,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,     6,     7,
       8,     9,    10,    35,    12,    -1,    -1,    -1,    16,    17,
      -1,    19,     6,     7,     8,     9,    10,    -1,    12,    -1,
      -1,    29,    -1,    17,    -1,    19,    -1,    35,    -1,    -1,
      -1,    -1,    11,    -1,    -1,    29,    -1,    -1,    -1,    -1,
      -1,    35,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    13,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    13,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    37,    39,     0,     6,     7,     8,     9,    10,    12,
      15,    17,    19,    29,    35,    38,    40,    41,    43,    12,
      15,    34,     8,    41,    39,     8,    12,    41,    41,    39,
      11,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,     4,     5,    41,    42,    39,     8,
      12,    13,    16,    12,    41,    41,    41,    41,    41,    41,
      41,    41,    41,    41,    41,    41,    41,    41,    41,    41,
      13,    14,    16,    42,    42,    13,    15,    41,    13,    13,
      15,    39,    34,    39,    16,     8,    16,    18,    20,     8,
      15,    17,    39,    16
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (parm, scanner, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, scanner)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, parm, scanner); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, parse_parm_t *parm, void *scanner)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, parm, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    parse_parm_t *parm;
    void *scanner;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (parm);
  YYUSE (scanner);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, parse_parm_t *parm, void *scanner)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, parm, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    parse_parm_t *parm;
    void *scanner;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, parm, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, parse_parm_t *parm, void *scanner)
#else
static void
yy_reduce_print (yyvsp, yyrule, parm, scanner)
    YYSTYPE *yyvsp;
    int yyrule;
    parse_parm_t *parm;
    void *scanner;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , parm, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, parm, scanner); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, parse_parm_t *parm, void *scanner)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, parm, scanner)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    parse_parm_t *parm;
    void *scanner;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (parm);
  YYUSE (scanner);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (parse_parm_t *parm, void *scanner);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (parse_parm_t *parm, void *scanner)
#else
int
yyparse (parm, scanner)
    parse_parm_t *parm;
    void *scanner;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 48 "parse.y"
    {
	  ts_tree_node_t *synt_tree = ts_tree_node_new(NODE_PRGM, ts_linenumber, ts_colnumber);
	  ts_tree_node_add_child(synt_tree, (yyvsp[(1) - (3)].Tree_node));
	  ts_tree_node_add_child(synt_tree, (yyvsp[(2) - (3)].Tree_node));
	  ts_tree_node_add_child(synt_tree, (yyvsp[(3) - (3)].Tree_node));
	  parm->result = synt_tree;
        }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 57 "parse.y"
    {
	 ts_tree_node_t *formula = ts_tree_node_new(NODE_FORMULA, ts_linenumber, ts_colnumber);
	 ts_tree_node_t *paralist = ts_tree_node_new(NODE_PARA_LIST, ts_linenumber, ts_colnumber);
	 ts_tree_node_add_child(formula, paralist);
	 ts_tree_node_add_child(formula, (yyvsp[(2) - (3)].Tree_node));
	 (yyval.Tree_node) = formula;
       }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 65 "parse.y"
    {
	 ts_tree_node_t *formula = ts_tree_node_new_end(NODE_FORMULA, (yyvsp[(1) - (4)].Tree_node), ts_linenumber, ts_colnumber);
	 ts_tree_node_t *paralist = ts_tree_node_new(NODE_PARA_LIST, ts_linenumber, ts_colnumber);
	 ts_tree_node_add_child(formula, paralist);
	 ts_tree_node_add_child(formula, (yyvsp[(3) - (4)].Tree_node));
	 (yyval.Tree_node) = formula;
       }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 73 "parse.y"
    {
	 ts_tree_node_t *formula = ts_tree_node_new_end(NODE_FORMULA, (yyvsp[(1) - (7)].Tree_node), ts_linenumber, ts_colnumber);
	 ts_tree_node_add_child(formula, (yyvsp[(3) - (7)].Tree_node));
	 ts_tree_node_add_child(formula, (yyvsp[(6) - (7)].Tree_node));
	 (yyval.Tree_node) = formula;
       }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 79 "parse.y"
    {(yyval.Tree_node) = ts_tree_node_new(NODE_FORMULA, ts_linenumber, ts_colnumber);}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 82 "parse.y"
    {
       ts_tree_node_add_child((yyvsp[(1) - (2)].Tree_node), (yyvsp[(2) - (2)].Tree_node));
       (yyval.Tree_node) = (yyvsp[(1) - (2)].Tree_node);
     }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 87 "parse.y"
    {
       (yyval.Tree_node) = ts_tree_node_new(NODE_STMTS, ts_linenumber, ts_colnumber);
     }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 91 "parse.y"
    { (yyval.Tree_node) = (yyvsp[(1) - (2)].Tree_node); }
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 92 "parse.y"
    {(yyval.Tree_node) = (yyvsp[(1) - (1)].Tree_node);}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 94 "parse.y"
    { (yyval.Tree_node) = (yyvsp[(2) - (3)].Tree_node); }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 96 "parse.y"
    {
       ts_tree_node_t *empty = ts_tree_node_new(NODE_EMPTY, ts_linenumber, ts_colnumber);
       (yyval.Tree_node) = ts_tree_node_new3(NODE_IFELSE, (yyvsp[(3) - (7)].Tree_node), (yyvsp[(6) - (7)].Tree_node), empty, ts_linenumber, ts_colnumber);
     }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 101 "parse.y"
    {
       (yyval.Tree_node) = ts_tree_node_new3(NODE_IFELSE, (yyvsp[(3) - (11)].Tree_node), (yyvsp[(6) - (11)].Tree_node), (yyvsp[(10) - (11)].Tree_node), ts_linenumber, ts_colnumber);
     }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 105 "parse.y"
    {
       (yyval.Tree_node) = ts_tree_node_new2(NODE_ADD, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber);
     }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 109 "parse.y"
    {
       (yyval.Tree_node) = ts_tree_node_new2(NODE_MINUS, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber);
     }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 113 "parse.y"
    {
       (yyval.Tree_node) = ts_tree_node_new2(NODE_MUL, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber);
     }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 117 "parse.y"
    {
       (yyval.Tree_node) = ts_tree_node_new2(NODE_DIV, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber);
     }
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 121 "parse.y"
    {
       (yyval.Tree_node) = ts_tree_node_new2(NODE_MOD, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber);
     }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 124 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new2(NODE_GT, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber); }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 125 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new2(NODE_LT, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber); }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 126 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new2(NODE_EQ, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber); }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 127 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new2(NODE_NEQ, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber); }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 128 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new2(NODE_LE, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber); }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 129 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new2(NODE_GE, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber); }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 130 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new2(NODE_AND, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 131 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new2(NODE_OR, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 132 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new1(NODE_NOT, (yyvsp[(2) - (2)].Tree_node), ts_linenumber, ts_colnumber); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 133 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new1(NODE_NEGATIVE, (yyvsp[(2) - (2)].Tree_node), ts_linenumber, ts_colnumber);}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 135 "parse.y"
    {
       ts_tree_node_t *fname = ts_tree_node_new_end(NODE_VAR, ".getOther", ts_linenumber, ts_colnumber);
       ts_tree_node_t *pl = ts_tree_node_new(NODE_PARA_LIST, ts_linenumber, ts_colnumber);
       ts_tree_node_t *tableName = ts_tree_node_new_end(NODE_STR, (yyvsp[(9) - (10)].Tree_node), ts_linenumber, ts_colnumber);
       ts_tree_node_t *otherFormulaName = ts_tree_node_new_end(NODE_STR, (yyvsp[(2) - (10)].Tree_node), ts_linenumber, ts_colnumber);
       ts_tree_node_t *otherVarName = ts_tree_node_new_end(NODE_STR, (yyvsp[(7) - (10)].Tree_node), ts_linenumber, ts_colnumber);
       ts_tree_node_add_child(pl, tableName);
       ts_tree_node_add_child(pl, otherFormulaName);
       ts_tree_node_add_child(pl, otherVarName);
       int i,nc = ((ts_tree_node_t*)(yyvsp[(4) - (10)].Tree_node))->nchild;
       for(i=0;i<nc;i++) {
	 ts_tree_node_add_child(pl, ((ts_tree_node_t*)(yyvsp[(4) - (10)].Tree_node))->child_ar[i]);
       }
       (yyval.Tree_node) = ts_tree_node_new2(NODE_FUNCTION, fname, pl, ts_linenumber, ts_colnumber);
       free((yyvsp[(2) - (10)].Tree_node));
       free((yyvsp[(7) - (10)].Tree_node));
       free((yyvsp[(9) - (10)].Tree_node));
     }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 153 "parse.y"
    { (yyval.Tree_node) = (yyvsp[(1) - (1)].Tree_node);}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 155 "parse.y"
    { 
       (yyval.Tree_node) = ts_tree_node_new2(NODE_OUTPUT_ASSIGN, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber);
     }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 159 "parse.y"
    {
       (yyval.Tree_node) = ts_tree_node_new2(NODE_ASSIGN, (yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber); 
     }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 163 "parse.y"
    {
       ts_tree_node_t *t1, *t2, *t3, *stmts, *iv;
       short funcid;
       ts_func_mgr_get_funcid(vm->func_mgr, (yyvsp[(1) - (4)].Tree_node), &funcid);
       if(ts_func_mgr_is_sfunc(vm->func_mgr, funcid)) {
	 int nsvar, i;
	 int *psvaridx_ar;
	 ts_func_mgr_get_sfunc_svar_ar(vm->func_mgr, funcid, &nsvar, &psvaridx_ar);
	 t1 = (yyvsp[(3) - (4)].Tree_node);
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
       ts_tree_node_t *tn = ts_tree_node_new_end(NODE_VAR, (yyvsp[(1) - (4)].Tree_node), ts_linenumber, ts_colnumber);
       (yyval.Tree_node) = ts_tree_node_new2(NODE_FUNCTION, tn, (yyvsp[(3) - (4)].Tree_node), ts_linenumber, ts_colnumber);
       free((yyvsp[(1) - (4)].Tree_node));
     }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 190 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new_end(NODE_STR, (yyvsp[(1) - (1)].Tree_node), ts_linenumber, ts_colnumber); free((yyvsp[(1) - (1)].Tree_node)); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 191 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new_end(NODE_VAR, (yyvsp[(1) - (1)].Tree_node), ts_linenumber, ts_colnumber); free((yyvsp[(1) - (1)].Tree_node)); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 192 "parse.y"
    {
         int end = strlen((yyvsp[(1) - (1)].Tree_node))-1;
	 if(((char*)(yyvsp[(1) - (1)].Tree_node))[end] == 'f') {
	   if(((char*)(yyvsp[(1) - (1)].Tree_node))[end-1] == 'l') {
	     (yyval.Tree_node) = ts_tree_node_new_end(NODE_DOUBLE, (yyvsp[(1) - (1)].Tree_node), ts_linenumber, ts_colnumber);
	   } else {
	     (yyval.Tree_node) = ts_tree_node_new_end(NODE_FLOAT, (yyvsp[(1) - (1)].Tree_node), ts_linenumber, ts_colnumber);
	   }
	 } else {
	   int n = valid_digits((yyvsp[(1) - (1)].Tree_node));
	   if(n > 7) {
	     (yyval.Tree_node) = ts_tree_node_new_end(NODE_DOUBLE, (yyvsp[(1) - (1)].Tree_node), ts_linenumber, ts_colnumber);
	   } else {
	     (yyval.Tree_node) = ts_tree_node_new_end(NODE_FLOAT, (yyvsp[(1) - (1)].Tree_node), ts_linenumber, ts_colnumber);
	   }
	 }
	 free((yyvsp[(1) - (1)].Tree_node));
       }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 210 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new_end(NODE_INT, (yyvsp[(1) - (1)].Tree_node), ts_linenumber, ts_colnumber);free((yyvsp[(1) - (1)].Tree_node));}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 211 "parse.y"
    {(yyval.Tree_node) = ts_tree_node_new_end(NODE_TIME_CONST, (yyvsp[(1) - (1)].Tree_node), ts_linenumber, ts_colnumber);free((yyvsp[(1) - (1)].Tree_node));}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 214 "parse.y"
    { ts_tree_node_add_child((yyvsp[(1) - (3)].Tree_node), (yyvsp[(3) - (3)].Tree_node)); (yyval.Tree_node) = (yyvsp[(1) - (3)].Tree_node); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 215 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new1(NODE_PARA_LIST, (yyvsp[(1) - (1)].Tree_node), ts_linenumber, ts_colnumber); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 216 "parse.y"
    {(yyval.Tree_node) = ts_tree_node_new(NODE_PARA_LIST, ts_linenumber, ts_colnumber);}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 220 "parse.y"
    {
       ts_tree_node_t *t1 = ts_tree_node_new_end(NODE_VAR, (yyvsp[(1) - (3)].Tree_node), ts_linenumber, ts_colnumber);
       ts_tree_node_t *t2 = ts_tree_node_new_end(NODE_VAR, (yyvsp[(3) - (3)].Tree_node), ts_linenumber, ts_colnumber);
       (yyval.Tree_node) = ts_tree_node_new2(NODE_DOT, t1, t2, ts_linenumber, ts_colnumber);
       free((yyvsp[(1) - (3)].Tree_node));
       free((yyvsp[(3) - (3)].Tree_node));
     }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 227 "parse.y"
    { (yyval.Tree_node) = ts_tree_node_new_end(NODE_VAR, (yyvsp[(1) - (1)].Tree_node), ts_linenumber, ts_colnumber); free((yyvsp[(1) - (1)].Tree_node)); }
    break;



/* Line 1455 of yacc.c  */
#line 1941 "parse.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (parm, scanner, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (parm, scanner, yymsg);
	  }
	else
	  {
	    yyerror (parm, scanner, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, parm, scanner);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, parm, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (parm, scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, parm, scanner);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, parm, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 229 "parse.y"


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


