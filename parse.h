
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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

/* Line 1676 of yacc.c  */
#line 27 "parse.y"

  char *value;
  void *Tree_node;



/* Line 1676 of yacc.c  */
#line 129 "parse.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




