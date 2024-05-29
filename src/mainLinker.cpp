#include <iostream>
using namespace std;
#include <regex>
#include <map>
#include "../inc/Linker.hpp"

int main(int argc, const char* argv[]){

  string izlazniFajl = "";
  vector<string> ulazniFajlovi; // Vazno je da ovo bude vektor da bismo pamtili fajlove u onom redosledu u kom su navedeni
  map<string, unsigned int> postavljeneSekcije;
  bool hex = false;
  bool relocatable = false;

  regex izraz("-place=([a-zA-Z_][a-zA-Z_0-9]*)@(0[xX][0-9A-Fa-f]+)");
  smatch odgovara;

  for(int i = 1; i < argc; i++){
    string argument = argv[i];
    if(argument == "-hex"){
      hex = true;
    }
    else if(argument == "-relocatable"){
      relocatable = true;
    }
    else if(argument == "-o"){
      i++; // Sledeci argument je ime izlaznog fajla
      if(i == argc){
        cout << "LINKER GRESKA: Iza opcije '-o' ne postoji ime fajla\n";
        exit(-1);
      }
      izlazniFajl = argv[i];
    }
    else if(regex_search(argument, odgovara, izraz)){
      postavljeneSekcije[odgovara.str(1)] = stol(odgovara.str(2), nullptr, 16);
    }
    else{
      ulazniFajlovi.push_back(argument);
    }
  }

  if(relocatable && hex){
    cout << "LINKER GRESKA: Morate odabrati tacno jednu opciju, odabrali ste dve\n";
    exit(-1);
  }
  if(!relocatable && !hex){
    cout << "LINKER GRESKA: Niste odabrali ni jednu opciju\n";
    exit(-1);
  }
  if(izlazniFajl == ""){
    cout << "LINKER GRESKA: Niste specificirali izlazni fajl\n";
    exit(-1);
  }

  Linker* lin = Linker::getInstance();
  lin->init(izlazniFajl, ulazniFajlovi, postavljeneSekcije, hex, relocatable);
  lin->linkuj();

  return 0;
}