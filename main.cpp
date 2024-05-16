#include <stdio.h>
#include "../inc/helper.hpp"
#include "../misc/lexer.hpp"


int main(){

  elementi_help = new vector<Leksicki_Element*>();
  argumenti_help = nullptr;
  fajl = fopen("./testiranje.txt" ,"r");
  if(!fajl) printf("Greska Input");
  yyin = fajl;
  yyparse();

  FILE* izlaz = fopen("izlaz.txt", "w");
  if(!izlaz) printf("Greska Output");

  for(int i = 0; i < elementi_help->size(); i++){
    elementi_help->at(i)->ispisi(izlaz);
  }
  return 0;
}