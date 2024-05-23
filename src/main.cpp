#include <stdio.h>
#include "../misc/lexer.hpp"
#include "../inc/Asembler.hpp"
#include <fstream>

int main(int argc, char* argv[]){

  const char* ulazIme;
  const char* izlazIme;

  if(argc < 4){
    cout << "Neispravni argumenti\n";
    exit(-1);
  }
  string opcija = argv[1];
  if(opcija != "-o"){
    cout << "Neispravna opcija: " << opcija << endl;
    exit(-1);
  }
  izlazIme = argv[2];
  ulazIme = argv[3];
  

  Asembler *asembler = Asembler::getInstance();
  asembler->init(ulazIme, izlazIme);
  asembler->prolaz();


  return 0;
}