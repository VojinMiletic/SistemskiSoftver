%{
  #include "../inc/helper.hpp"
  #include "../inc/Operand.hpp"
  #include <stdio.h>
  #include <vector>
  int yylex(void);
  void yyerror(const char*);
  extern int broj_linije;
%}

%output "parser.cpp"
%defines "parser.hpp"
%union {
  int broj;
  char *simbol;
  struct argument *arg;
  class Operand* opr;
}

%token <simbol> TOKEN_GLOBAL 
%token <simbol> TOKEN_EXTERN 
%token <simbol> TOKEN_SECTION
%token <simbol> TOKEN_WORD
%token <simbol> TOKEN_SKIP 
%token <simbol> TOKEN_ASCII
%token <simbol> TOKEN_END 

%token <simbol> TOKEN_HALT
%token <simbol> TOKEN_INT
%token <simbol> TOKEN_IRET
%token <simbol> TOKEN_CALL
%token <simbol> TOKEN_RET
%token <simbol> TOKEN_JMP
%token <simbol> TOKEN_BEQ
%token <simbol> TOKEN_BNE
%token <simbol> TOKEN_BGT
%token <simbol> TOKEN_PUSH
%token <simbol> TOKEN_POP
%token <simbol> TOKEN_XCHG
%token <simbol> TOKEN_ADD
%token <simbol> TOKEN_SUB
%token <simbol> TOKEN_MUL
%token <simbol> TOKEN_DIV
%token <simbol> TOKEN_NOT
%token <simbol> TOKEN_AND
%token <simbol> TOKEN_OR
%token <simbol> TOKEN_XOR
%token <simbol> TOKEN_SHL
%token <simbol> TOKEN_SHR
%token <simbol> TOKEN_LD
%token <simbol> TOKEN_ST
%token <simbol> TOKEN_CSRRD
%token <simbol> TOKEN_CSRWR

%token <broj> TOKEN_GP_REGISTAR
%token <broj> TOKEN_CS_REGISTAR

%token TOKEN_ZAREZ
%token ENDL
%token TOKEN_COM
%token TOKEN_IMM
%token TOKEN_LUZ
%token TOKEN_DUZ
%token TOKEN_PLUS

%token <simbol> TOKEN_SIMBOL
%token <simbol> TOKEN_LABELA
%token <broj> TOKEN_LITERAL
%token <simbol> TOKEN_STRING

%type <arg> lista_simbola;
%type <arg> lista_literala_ili_simbola;
%type <opr> operand;

%%

prog:
line ENDLS
| prog line ENDLS
| prog line
{}
;

ENDLS:
ENDLS ENDL
{
  broj_linije++;
}
| ENDL 
{
  broj_linije++;
}
;

line:
labela operacija
| labela
| operacija
| TOKEN_COM
;

operacija:
direktiva
| instrukcija
;

instrukcija:
  TOKEN_HALT
  { (new Instrukcija(TOKEN_HALT, broj_linije, $1))->prolaz();}
| TOKEN_INT
  {(new Instrukcija(TOKEN_INT, broj_linije, $1))->prolaz();}  
| TOKEN_IRET
  {(new Instrukcija(TOKEN_IRET, broj_linije, $1))->prolaz();}
| TOKEN_CALL operand
  {(new Instrukcija(TOKEN_CALL, broj_linije, $1, $2))->prolaz();}
| TOKEN_RET
  {(new Instrukcija(TOKEN_RET, broj_linije, $1))->prolaz();}
| TOKEN_JMP operand
  {(new Instrukcija(TOKEN_JMP, broj_linije, $1, $2))->prolaz();}
| TOKEN_BEQ TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR TOKEN_ZAREZ operand
  {(new Instrukcija(TOKEN_BEQ,broj_linije,$1,$6,$2,$4))->prolaz();}
| TOKEN_BNE TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR TOKEN_ZAREZ operand
  {(new Instrukcija(TOKEN_BNE,broj_linije,$1,$6,$2,$4))->prolaz();}  
| TOKEN_BGT TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR TOKEN_ZAREZ operand
  {(new Instrukcija(TOKEN_BGT,broj_linije,$1,$6,$2,$4))->prolaz();}
| TOKEN_PUSH TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_PUSH, broj_linije, $1,nullptr, $2))->prolaz();}
| TOKEN_POP TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_POP, broj_linije, $1,nullptr, $2))->prolaz();}
| TOKEN_XCHG TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_XCHG,broj_linije,$1,nullptr,$2,$4))->prolaz();}
| TOKEN_ADD TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_ADD,broj_linije,$1,nullptr,$2,$4))->prolaz();}
| TOKEN_SUB TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_SUB,broj_linije,$1,nullptr,$2,$4))->prolaz();}
| TOKEN_MUL TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_MUL,broj_linije,$1,nullptr,$2,$4))->prolaz();}
| TOKEN_DIV TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_DIV,broj_linije,$1,nullptr,$2,$4))->prolaz();}
| TOKEN_NOT TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_NOT,broj_linije,$1,nullptr,$2))->prolaz();}
| TOKEN_AND TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_AND,broj_linije,$1,nullptr,$2,$4))->prolaz();}
| TOKEN_OR TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_OR,broj_linije,$1,nullptr,$2,$4))->prolaz();}
| TOKEN_XOR TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_XOR,broj_linije,$1,nullptr,$2,$4))->prolaz();}
| TOKEN_SHL TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_SHL,broj_linije,$1,nullptr,$2,$4))->prolaz();}
| TOKEN_SHR TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_SHR,broj_linije,$1,nullptr,$2,$4))->prolaz();}
| TOKEN_LD operand TOKEN_ZAREZ TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_LD, broj_linije, $1, $2, $4))->prolaz();}
| TOKEN_ST TOKEN_GP_REGISTAR TOKEN_ZAREZ operand
  {(new Instrukcija(TOKEN_ST, broj_linije, $1, $4, $2))->prolaz();}
| TOKEN_CSRRD TOKEN_CS_REGISTAR TOKEN_ZAREZ TOKEN_GP_REGISTAR
  {(new Instrukcija(TOKEN_CSRRD,broj_linije, $1, nullptr, $4, -1, $2))->prolaz();}
| TOKEN_CSRWR TOKEN_GP_REGISTAR TOKEN_ZAREZ TOKEN_CS_REGISTAR
  {(new Instrukcija(TOKEN_CSRWR, broj_linije, $1, nullptr, $2, -1, $4))->prolaz();}
;


operand:
  TOKEN_IMM TOKEN_LITERAL
  { $$ = new Operand(IMMED, $2);}
| TOKEN_IMM TOKEN_SIMBOL
  { $$ = new Operand(IMMED, -1, $2);}
| TOKEN_LITERAL
  { $$ = new Operand(MEMDIR, $1);}
| TOKEN_SIMBOL
  { $$ = new Operand(MEMDIR, -1, $1);}
| TOKEN_GP_REGISTAR
  { $$ = new Operand(REGDIR, -1, nullptr, $1);}
| TOKEN_CS_REGISTAR
  { $$ = new Operand(REGDIR, -1, nullptr, -1, $1, true);}
| TOKEN_LUZ TOKEN_GP_REGISTAR TOKEN_DUZ
  { $$ = new Operand(REGIND, -1, nullptr, $2);}
| TOKEN_LUZ TOKEN_CS_REGISTAR TOKEN_DUZ
  { $$ = new Operand(REGIND, -1, nullptr, -1, $2, true);}
| TOKEN_LUZ TOKEN_GP_REGISTAR TOKEN_PLUS TOKEN_LITERAL TOKEN_DUZ
  {$$ = new Operand(REGINDPOM, $4, nullptr, $2);}
| TOKEN_LUZ TOKEN_CS_REGISTAR TOKEN_PLUS TOKEN_LITERAL TOKEN_DUZ
  {$$ = new Operand(REGINDPOM, $4, nullptr,-1, $2, true);}
| TOKEN_LUZ TOKEN_GP_REGISTAR TOKEN_PLUS TOKEN_SIMBOL TOKEN_DUZ
  {$$ = new Operand(REGINDPOM,-1,$4,$2);}
| TOKEN_LUZ TOKEN_CS_REGISTAR TOKEN_PLUS TOKEN_SIMBOL TOKEN_DUZ
  {$$ = new Operand(REGINDPOM,-1,$4,-1,$2,true);}
;

labela:
  TOKEN_LABELA
  {
    string mnemonik = $1;
    mnemonik.pop_back();
    (new Direktiva(TOKEN_LABELA,broj_linije, mnemonik))->prolaz();
  }
;

// Direktiva(yytokentype token, int brojLinije, char* mnemonik, vector<pair<bool,argument>> *argumenti = nullptr)

direktiva:
  TOKEN_GLOBAL lista_simbola
  {
    struct argument* lista_argumenata = $2;
    vector<pair<bool,Unija>>* vektor_argumenata = new vector<pair<bool,Unija>>();
    while(lista_argumenata){
      vektor_argumenata->push_back(make_pair(false, (Unija){.simbol = lista_argumenata->simbol}));
      lista_argumenata = lista_argumenata->next;
    }
    freeArgument(argumenti_help);
    argumenti_help = nullptr;
    (new Direktiva(TOKEN_GLOBAL, broj_linije,$1,vektor_argumenata))->prolaz();
  }
| TOKEN_EXTERN lista_simbola
  {
    struct argument* lista_argumenata = $2;
    vector<pair<bool,Unija>>* vektor_argumenata = new vector<pair<bool,Unija>>();
    while(lista_argumenata){
      vektor_argumenata->push_back(make_pair(false, (Unija){.simbol = lista_argumenata->simbol}));
      lista_argumenata = lista_argumenata->next;
    }
    freeArgument(argumenti_help);
    argumenti_help = nullptr;
    (new Direktiva(TOKEN_EXTERN, broj_linije,$1,vektor_argumenata))->prolaz();
  }
| TOKEN_SECTION TOKEN_SIMBOL
  {
    vector<pair<bool,Unija>>* vektor_argumenata = new vector<pair<bool,Unija>>();
    vektor_argumenata->push_back(make_pair(false, (Unija){.simbol = $2}));
    (new Direktiva(TOKEN_SECTION, broj_linije,$1, vektor_argumenata))->prolaz();
  }
| TOKEN_WORD lista_literala_ili_simbola
  {
    struct argument* lista_argumenata = $2;
    vector<pair<bool,Unija>>* vektor_argumenata = new vector<pair<bool,Unija>>();
    while(lista_argumenata){
      if(lista_argumenata->simbol){
        vektor_argumenata->push_back(make_pair(false, (Unija){.simbol = lista_argumenata->simbol}));
      }
      else{
        vektor_argumenata->push_back(make_pair(true, (Unija){.broj = lista_argumenata->broj}));
      }
      lista_argumenata = lista_argumenata->next;
    }
    freeArgument(argumenti_help);
    argumenti_help = nullptr;
    (new Direktiva(TOKEN_WORD, broj_linije, $1, vektor_argumenata))->prolaz();
  }
| TOKEN_SKIP TOKEN_LITERAL
  {
    vector<pair<bool,Unija>>* vektor_argumenata = new vector<pair<bool,Unija>>();
    vektor_argumenata->push_back(make_pair(true,(Unija){.broj = $2}));
    (new Direktiva(TOKEN_SKIP, broj_linije, $1, vektor_argumenata))->prolaz();
  }
| TOKEN_ASCII TOKEN_STRING
  {
    vector<pair<bool,Unija>>* vektor_argumenata = new vector<pair<bool,Unija>>();
    vektor_argumenata->push_back(make_pair(false, (Unija){.simbol = $2}));
    (new Direktiva(TOKEN_ASCII, broj_linije, $1, vektor_argumenata))->prolaz();
  }  
| TOKEN_END
  {
    (new Direktiva(TOKEN_END, broj_linije, $1))->prolaz();
  }
;


lista_literala_ili_simbola:
  TOKEN_LITERAL
  {
    if(!argumenti_help){
      argumenti_help = napraviArgument($1, nullptr);
    }
    else{
      argumenti_help->next = napraviArgument($1, nullptr);
      argumenti_help = argumenti_help->next;
    }
    $$ = argumenti_help;
  }
| TOKEN_SIMBOL
  {
    if(!argumenti_help){
      argumenti_help = napraviArgument(-1, $1);
    }
    else{
      argumenti_help->next = napraviArgument(-1, $1);
      argumenti_help = argumenti_help->next;
    }
    $$ = argumenti_help;
  }
| lista_literala_ili_simbola TOKEN_ZAREZ TOKEN_LITERAL
  {
    if(!argumenti_help){
      argumenti_help = napraviArgument($3, nullptr);
    }
    else{
      argumenti_help->next = napraviArgument($3, nullptr);
      argumenti_help = argumenti_help->next;
    }
  }
| lista_literala_ili_simbola TOKEN_ZAREZ TOKEN_SIMBOL
  {
    if(!argumenti_help){
      argumenti_help = napraviArgument(-1, $3);
    }
    else{
      argumenti_help->next = napraviArgument(-1, $3);
      argumenti_help = argumenti_help->next;
    }
  }
;

lista_simbola:
  TOKEN_SIMBOL
  {
    if(!argumenti_help){
      argumenti_help = napraviArgument(-1,$1);
    }
    else{
      argumenti_help->next = napraviArgument(-1,$1);
      argumenti_help = argumenti_help->next;
    }
    $$ = argumenti_help;
  }
| lista_simbola TOKEN_ZAREZ TOKEN_SIMBOL
  {
    if(!argumenti_help){
      argumenti_help = napraviArgument(-1,$3);
    }
    else{
      argumenti_help->next = napraviArgument(-1,$3);
      argumenti_help = argumenti_help->next;
    }
  }

%%