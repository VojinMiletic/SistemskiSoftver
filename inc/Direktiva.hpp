#ifndef DIREKTIVA_HPP
#define DIREKTIVA_HPP

#include <iostream>
#include <vector>
using namespace std;
#include "LeksickiElement.hpp"

union Unija{
  int broj;
  char *simbol;
};


class Direktiva : public Leksicki_Element{

  vector<pair<bool,Unija>> *argumenti; // false=> u pitanju je simbol, true=>u pitanju je broj
  string mnemonik;
  yytokentype token;
public:
  Direktiva(yytokentype token, int brojLinije, string mnemonik, vector<pair<bool,Unija>> *argumenti = nullptr) : 
      Leksicki_Element(brojLinije)
      {
        this->argumenti = argumenti;
        this->mnemonik = mnemonik;
        this->token = token;
      }

  void ispisiDirektivu(FILE* fajl){
    
    fprintf(fajl, "%d %s ", brojLinije, mnemonik.c_str());
    cout << brojLinije << " " << mnemonik << " ";
    if(argumenti != nullptr){
      for(int i = 0; i < argumenti->size(); i++){
        if(i != 0){
          fprintf(fajl, ", ");
          cout << ", ";
        }
        if(argumenti->at(i).first){
          fprintf(fajl, "%d ", argumenti->at(i).second.broj);
          cout << argumenti->at(i).second.broj;
        }
        else{
          fprintf(fajl, "%s ", argumenti->at(i).second.simbol);
          cout << argumenti->at(i).second.simbol;
        }
      }
    }
  }

  void ispisi(FILE* fajl) override{
    this->ispisiDirektivu(fajl);
    cout << "\n";
    fprintf(fajl, "\n");
  }

  bool prolaz() override;

};


#endif