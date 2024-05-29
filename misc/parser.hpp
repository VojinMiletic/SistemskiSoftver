/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PARSER_HPP_INCLUDED
# define YY_YY_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOKEN_GLOBAL = 258,
    TOKEN_EXTERN = 259,
    TOKEN_SECTION = 260,
    TOKEN_WORD = 261,
    TOKEN_SKIP = 262,
    TOKEN_ASCII = 263,
    TOKEN_END = 264,
    TOKEN_HALT = 265,
    TOKEN_INT = 266,
    TOKEN_IRET = 267,
    TOKEN_CALL = 268,
    TOKEN_RET = 269,
    TOKEN_JMP = 270,
    TOKEN_BEQ = 271,
    TOKEN_BNE = 272,
    TOKEN_BGT = 273,
    TOKEN_PUSH = 274,
    TOKEN_POP = 275,
    TOKEN_XCHG = 276,
    TOKEN_ADD = 277,
    TOKEN_SUB = 278,
    TOKEN_MUL = 279,
    TOKEN_DIV = 280,
    TOKEN_NOT = 281,
    TOKEN_AND = 282,
    TOKEN_OR = 283,
    TOKEN_XOR = 284,
    TOKEN_SHL = 285,
    TOKEN_SHR = 286,
    TOKEN_LD = 287,
    TOKEN_ST = 288,
    TOKEN_CSRRD = 289,
    TOKEN_CSRWR = 290,
    TOKEN_GP_REGISTAR = 291,
    TOKEN_CS_REGISTAR = 292,
    TOKEN_ZAREZ = 293,
    ENDL = 294,
    TOKEN_COM = 295,
    TOKEN_IMM = 296,
    TOKEN_LUZ = 297,
    TOKEN_DUZ = 298,
    TOKEN_PLUS = 299,
    TOKEN_SIMBOL = 300,
    TOKEN_LABELA = 301,
    TOKEN_LITERAL = 302,
    TOKEN_STRING = 303
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 13 "./misc/parserKopija.y"

  int broj;
  char *simbol;
  struct argument *arg;
  class Operand* opr;

#line 113 "parser.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_HPP_INCLUDED  */
