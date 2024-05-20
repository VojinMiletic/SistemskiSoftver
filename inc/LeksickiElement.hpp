#ifndef LEKSICKI_ELEMENT_HPP
#define LEKSICKI_ELEMENT_HPP

#include <iostream>
using namespace std;
#include "../misc/parser.hpp"

class Leksicki_Element{
  public:
    Leksicki_Element(int linija){brojLinije = linija;}
    virtual void ispisi(FILE* fajl){}
    virtual bool prolaz() = 0;

  protected:
    int brojLinije;  
};

#endif