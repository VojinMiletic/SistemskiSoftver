%{
#include "parser.hpp"
#include "../inc/helper.hpp"
#include <string.h>
int broj_linije = 1;
%}

%option outfile="lexer.cpp" header-file="lexer.hpp"

%option noyywrap

LUZ "["
DUZ "]"
PLUS "+"
ZAREZ ","

GLOBAL \.global
EXTERN \.extern
SECTION \.section
WORD \.word
SKIP \.skip
ASCII \.ascii
END \.end
HEX 0[Xx][0-9A-Fa-f]+ */
COM #.*
BLANCO [ \r\t]*
STRING \"[^"]+\"
LITERAL_DECIMALNI -?[0-9]+
LITERAL_HEKSA 0x[0-9A-Fa-f]+
SIMBOL [a-zA-Z][a-zA-Z0-9_]*
LABELA [a-zA-Z][a-zA-Z0-9_]*:
GP_REGISTAR %(r[0-9]+|sp|pc)
CS_REGISTAR %(status|handler|cause)

%%
{BLANCO} {}
{COM} { return TOKEN_COM; }
\$ { return TOKEN_IMM; }
{LUZ} { return TOKEN_LUZ; }
{DUZ} { return TOKEN_DUZ; }
{PLUS} { return TOKEN_PLUS; }
{GLOBAL} {
  yylval.simbol =  kopiraj_string(yytext);
  return TOKEN_GLOBAL; 
}
{EXTERN} { 
  yylval.simbol =  kopiraj_string(yytext);
  return TOKEN_EXTERN; 
}
{SECTION} { 
  yylval.simbol =  kopiraj_string(yytext);
  return TOKEN_SECTION; 
}
{WORD} { 
  yylval.simbol =  kopiraj_string(yytext);
  return TOKEN_WORD; 
}
{SKIP} { 
  yylval.simbol =  kopiraj_string(yytext);
  return TOKEN_SKIP; 
}
{ASCII} { 
  yylval.simbol =  kopiraj_string(yytext);
  return TOKEN_ASCII; 
}
{END} { 
  yylval.simbol =  kopiraj_string(yytext);
  return TOKEN_END; 
}
{LITERAL_DECIMALNI} {
  yylval.broj = atoi(yytext);
  return TOKEN_LITERAL;
}
{LITERAL_HEKSA} {
  yylval.broj = strtol(yytext, NULL, 16);
  return TOKEN_LITERAL;
}
{STRING} {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_STRING;
}
{LABELA} {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_LABELA;
}
halt {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_HALT;
}
int {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_INT;
}
iret {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_IRET;
}
call {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_CALL;
}
ret {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_RET;
}
jmp {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_JMP;
}
beq {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_BEQ;
}
bne {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_BNE;
}
bgt {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_BGT;
}
push {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_PUSH;
}
pop {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_POP;
}
xchg {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_XCHG;
}
add {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_ADD;
}
sub {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_SUB;
}
mul {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_MUL;
}
div {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_DIV;
}
not {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_NOT;
}
and {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_AND;
}
or {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_OR;
}
xor {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_XOR;
}
shl {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_SHL;
}
shr {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_SHR;
}
ld {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_LD;
}
st {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_ST;
}
csrrd {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_CSRRD;
}
csrwr {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_CSRWR;
}
{GP_REGISTAR} {
  if(!strcmp(yytext, "%sp")){
    yylval.broj = 14;
    return TOKEN_GP_REGISTAR;
  }
  if(!strcmp(yytext, "%pc")){
    yylval.broj = 15;
    return TOKEN_GP_REGISTAR;
  }
  yylval.broj = atoi(yytext+2);
  return TOKEN_GP_REGISTAR;
}
{CS_REGISTAR} {
  if(!strcmp(yytext, "%status")){
    yylval.broj = 0;
    return TOKEN_CS_REGISTAR;
  }
  if(!strcmp(yytext, "%handler")){
    yylval.broj = 1;
    return TOKEN_CS_REGISTAR;
  }
  if(!strcmp(yytext, "%cause")){
    yylval.broj = 2;
    return TOKEN_CS_REGISTAR;
  }
}
{SIMBOL} {
  yylval.simbol = kopiraj_string(yytext);
  return TOKEN_SIMBOL;
}
{ZAREZ} { return TOKEN_ZAREZ; }
\n {return ENDL;}
. {}
%%


int yyerror(const char* msg)
{
  fprintf(stderr, "Greska u lekseru: %s\n", msg);
  return 0;
}