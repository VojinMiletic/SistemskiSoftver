#include "../inc/Direktiva.hpp"
#include "../inc/Asembler.hpp"

bool Direktiva::prolaz(){
  switch(token){
    case TOKEN_LABELA:{
      
      return Asembler::getInstance()->obradiLabelu(mnemonik, brojLinije);
    }
    case TOKEN_GLOBAL:{
      for(int i = 0; i < argumenti->size(); i++){
         Asembler::getInstance()->obradiGlobal(argumenti->at(i).second.simbol, brojLinije);
      }
      return true;
    }
    case TOKEN_EXTERN:{
      for(int i = 0; i < argumenti->size(); i++){
       Asembler::getInstance()->obradiExtern(argumenti->at(i).second.simbol, brojLinije);
      }
      return true;
    }
    case TOKEN_WORD:{
      for(int i = 0; i < argumenti->size(); i++){
       Asembler::getInstance()->obradiWord(argumenti->at(i), brojLinije);
      }
      return true;
    }
    case TOKEN_SECTION:{
      return Asembler::getInstance()->obradiSekciju(argumenti->at(0).second.simbol, brojLinije);
    }
    case TOKEN_SKIP:{
      return Asembler::getInstance()->obradiSkip(argumenti->at(0).second.broj, brojLinije);
    }
    case TOKEN_END:{
      return Asembler::getInstance()->obradiEnd(brojLinije);
    }
    case TOKEN_ASCII:{
      string tekst = argumenti->at(0).second.simbol + 1; // Da preskocimo otvoren znak navoda
      tekst.pop_back(); // Da odbacimo zatvoren znak navoda
      string novi;
      int cnt = 0;
      while(cnt < tekst.size()){
        if(cnt < tekst.size() - 1 && tekst.at(cnt) == '\\'){
          switch(tekst.at(cnt + 1)){
            case 'n':
              novi.push_back('\n'); break;
            case 't':
              novi.push_back('\t'); break;
          }
          cnt += 2;
        }
        else{
          novi.push_back(tekst.at(cnt));
          cnt++;
        }
      }
      return Asembler::getInstance()->obradiAscii(novi, brojLinije);
    }
  }
  return true;
}