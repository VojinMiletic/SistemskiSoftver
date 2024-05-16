#ifndef OPERAND_HPP
#define OPERAND_HPP
#include <stdio.h>
#include <iostream>
using namespace std;

enum TipAdresiranja{
    IMMED = 0,
    MEMDIR,
    REGDIR,
    REGIND,
    REGINDPOM
};


class Operand{

  TipAdresiranja tipAdresiranja;
  int gpr;
  int csr;
  bool isCsr;
  int literal;
  char* simbol;

public:
  Operand(TipAdresiranja tipAdresiranja, int literal = -1, char* simbol = nullptr, int gpr = -1, int csr = -1, bool isCsr = false){
    this->tipAdresiranja = tipAdresiranja;
    this->literal = literal;
    this->simbol = simbol;
    this->gpr = gpr;
    this->csr = csr;
    this->isCsr = isCsr;
  }
  void ispisiOpernand(FILE* fajl){
    switch(tipAdresiranja){
      case MEMDIR:
        fprintf(fajl, " MEMDIR ");
        cout << " MEMDIR ";
        break;
      case IMMED:
        fprintf(fajl, " IMMED ");
        cout << " IMMED ";
        break;
      case REGDIR:
        fprintf(fajl, " REGDIR ");
        cout << " REGDIR ";
        break;  
      case REGIND:
      fprintf(fajl, " REGIND ");
      cout << " REGIND ";
      break; 
      case REGINDPOM:
      fprintf(fajl, " REGINDPOM ");
      cout << " REGINDPOM ";
      break;  
    }
    
    if(literal != -1){
      cout << literal << " ";
      fprintf(fajl, "%d ", literal);
    }
    if(simbol){
      cout << simbol << " ";
      fprintf(fajl, "%s ", simbol);
    }
    if(gpr != -1){
      cout << "r" <<  gpr << " ";
      fprintf(fajl, "gpr%d ", gpr);
    }
    if(csr != -1){
      cout << "csr" <<  csr << " ";
      fprintf(fajl, "csr%d ", csr);
    }
  }
};

#endif