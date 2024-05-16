#ifndef INSTRUKCIJA_HPP
#define INSTRUKCIJA_HPP

#include "LeksickiElement.hpp"
#include "Operand.hpp"

class Instrukcija : public Leksicki_Element{
  char* mnemonik;
  Operand* operand;
  int gpr1;
  int gpr2;
  int csr;

  yytokentype token; 

public:
  Instrukcija(yytokentype token, int brojLinije, char* mnemonik, Operand* operand = nullptr, int gpr1 = -1, int gpr2 = -1, int csr = -1) 
        : Leksicki_Element(brojLinije)
        {
          this->token = token;
          this->mnemonik = mnemonik;
          this->operand = operand;
          this->gpr1 = gpr1;
          this->gpr2 = gpr2;
          this->csr = csr;
        }
  
  void ispisiInstrikciju(FILE* fajl){
    if(mnemonik != nullptr){
      cout << mnemonik << " ";
      fprintf(fajl, "%d %s ", brojLinije, mnemonik);
      if(gpr1 != -1){
        cout << "r" << gpr1 << " ";
        fprintf(fajl, "r%d ", gpr1);
      }
      if(gpr2 != -1){
        cout <<"r" << gpr2 << " ";
        fprintf(fajl, "r%d ", gpr2);
      }
      if(csr != -1){
        cout << "csr" << csr << " ";
        fprintf(fajl, "csr%d ", csr);
      }
    }
  }

  void ispisi(FILE* fajl) override{
    this->ispisiInstrikciju(fajl);
    if(operand != nullptr){
      operand->ispisiOpernand(fajl);
    }
    fprintf(fajl, "\n");
    cout << "\n";
  }

};

#endif