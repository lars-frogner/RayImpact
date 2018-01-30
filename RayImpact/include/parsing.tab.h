/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
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

#ifndef YY_YY_C_USERS_LARS_RENDERING_IMPACT_RAYIMPACT_INCLUDE_PARSING_TAB_H_INCLUDED
# define YY_YY_C_USERS_LARS_RENDERING_IMPACT_RAYIMPACT_INCLUDE_PARSING_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     BOOL_TYPE = 258,
     INT_TYPE = 259,
     FLOAT_TYPE = 260,
     STRING_TYPE = 261,
     POINT2_TYPE = 262,
     VECTOR2_TYPE = 263,
     POINT3_TYPE = 264,
     VECTOR3_TYPE = 265,
     NORMAL3_TYPE = 266,
     RGB_TYPE = 267,
     XYZ_TYPE = 268,
     SPD_TYPE = 269,
     SPDF_TYPE = 270,
     BOOL_ARR_TYPE = 271,
     INT_ARR_TYPE = 272,
     FLOAT_ARR_TYPE = 273,
     STRING_ARR_TYPE = 274,
     POINT2_ARR_TYPE = 275,
     VECTOR2_ARR_TYPE = 276,
     POINT3_ARR_TYPE = 277,
     VECTOR3_ARR_TYPE = 278,
     NORMAL3_ARR_TYPE = 279,
     RGB_ARR_TYPE = 280,
     XYZ_ARR_TYPE = 281,
     SPD_ARR_TYPE = 282,
     SPDF_ARR_TYPE = 283,
     ALPHANUMERIC_VAL = 284,
     BOOL_VAL = 285,
     INT_VAL = 286,
     FLOAT_VAL = 287,
     STRING_VAL = 288,
     DELIM = 289,
     ARRAY_BEGIN = 290,
     ARRAY_END = 291,
     VEC_BEGIN = 292,
     VEC_END = 293,
     PARAM_BEGIN = 294,
     PARAM_END = 295,
     LINE_END = 296,
     INVALID = 297
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 8 "parsing.y"

	char s[YYLVAL_STR_LEN];
	int i;
	double f;


/* Line 2058 of yacc.c  */
#line 106 "C:\\Users\\Lars\\Rendering\\Impact\\RayImpact\\include\\parsing.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_C_USERS_LARS_RENDERING_IMPACT_RAYIMPACT_INCLUDE_PARSING_TAB_H_INCLUDED  */
