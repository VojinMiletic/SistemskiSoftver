#include "../inc/Instrukcija.hpp"
#include "../inc/Asembler.hpp"

bool Instrukcija::prolaz(){
  //yytokentype token, int brLinije, char* mnemonik, Operand* operand, int gpr1, int gpr2, int csr
  return Asembler::getInstance()->obradiInstrukciju(token, brojLinije, mnemonik, operand, gpr1, gpr2, csr);
}

