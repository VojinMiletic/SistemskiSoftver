#include <iostream>
using namespace std;
#include "../inc/Emulator.hpp"

int main(int argc, const char* argv[]){

  if(argc < 2){
    cout << "Nedovoljno argumenata!\n";
    exit(-1);
  }

  string ulazniFajl = argv[1];

  Emulator *em = Emulator::getInstance();
  em->init(ulazniFajl);
  em->emuliraj();

  return 0;
}