#ifndef ASEMBLER_HPP
#define ASEMBLER_HPP
#include "../inc/helper.hpp"
#include "../misc/lexer.hpp"
#include "../inc/Operand.hpp"
#include "../inc/LeksickiElement.hpp"
#include "../inc/Direktiva.hpp"
#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include  <algorithm>
using namespace std;


enum kodOperacije{
  HALT = 0x00,
  INT = 0x10,
  CALL_IMM = 0x20,
  CALL_MEM = 0x21,
  JMP_IMM = 0x30,
  BEQ_IMM = 0x31,
  BNE_IMM = 0x32,
  BGT_IMM = 0x33,
  JMP_MEM = 0x38,
  BEQ_MEM = 0x39,
  BNE_MEM = 0x3A,
  BGT_MEM = 0x3B,
  XCHG = 0x40,
  ADD = 0x50,
  SUB = 0x51,
  MUL = 0x52,
  DIV = 0x53,
  NOT = 0x60,
  AND = 0x61,
  OR = 0x62,
  XOR = 0x63,
  SHL = 0x70,
  SHR = 0x71,
  ST_MEM = 0x80,
  ST_MEM_MEM = 0x82,
  PUSH = 0x81,
  CSRRD = 0x90,
  LD_REG = 0x91,
  LD_MEM_REG = 0x92,
  POP = 0x93,
  CSRWR = 0x94,
  CSRWR_OR = 0x95,
  CSRWR_MEM = 0x96,
  CSRWR_MEM_POSTINC = 0x97
};




class Asembler{

  friend class Direktiva;
  friend class Instrukcija;
  static Asembler* instanca;
  

  struct UlazUTabeluLiterala{
    int redniBroj;
  };  

  struct UlazUTabeluObracanjaUnapred{
    string trenutnaSekcija;
    int trenutniLC;
    yytokentype instrukcija;  // Ovde se moze naci i TOKEN_WORD, tada je operand null a relokacioni zapis se(ako je potrebno)
                              // pravi na mestu iza WORD reci(trenutna vrednost LC-a).U ostalim slucajevima rel zapis
                              // se pravi u odnosu na ulaz u bazan literala a na mesto LC-a se ugradjuje pomeraj do ulaza u bazenu
    Operand* operand;
    int redBrSimbolaUBazenu; 
    string imeSimbola;
    int gpr1;
    int gpr2;
    int csr;
  };

  struct UlazUTabeluSekcija{
    string imeSekcije;
    int velicina;
    int idSekcije;
    map<int,UlazUTabeluLiterala> tabelaLiterarnihKonstanti;
    map<string,UlazUTabeluLiterala> tabelaSimbolickihKonstanti;
    vector<UlazUTabeluObracanjaUnapred>* tabelaObracanjaUnapred;
    int redniBrojKonstante = 0; // Ovo cemo da inkrementiramo pri ubacivanju literala/simbola u tabele konstanti 
                              // da bismo znali na koju poyiciju da ga smestimo u bazenu
  };

  

  struct UlazUTabeluSimbola{
    int idSimbola;
    string imeSimbola;
    int vrednost;
    bool isGlobal;
    bool isExtern;
    bool definisan;
    string sekcija;
    vector<UlazUTabeluObracanjaUnapred>* tabelaObracanjaUnapred;
  };

  struct UlazURelokacionuTabelu{
    int offset;
    string tip;
    string simbol;
    int redBrSimbola;
    int addend;
  };

  map<string, UlazUTabeluSimbola> tabelaSimbola; // kljuc je ime simbola
  map<string, UlazUTabeluSekcija> tabelaSekcija; // kljuc je ime sekcije
  map<string, vector<UlazURelokacionuTabelu>> tabelaRelokacija; // kljuc je ime sekcije
  map<string, stringstream> kodSekcije; // kljuc je ime sekcije

  
  int redniBrojSimbola = 0;
  int redniBrojSekcije = 0;
  int LC;
  string trenutnaSekcija;
 
  Asembler();

  bool obradiInstrukciju(yytokentype token, int brLinije, char* mnemonik, Operand* operand, int gpr1, int gpr2, int csr);
  
  bool obradiGlobal(string imeSimbola, int brLinije);
  bool obradiExtern(string imeSimbola, int brLinije);
  bool obradiSkip(int vrednostLiterala, int brLinije);
  bool obradiWord(pair<bool, Unija> argument, int brLinije);
  bool obradiAscii(string tekst, int brLinije);
  bool obradiEnd(int brojLinije);
  bool obradiLabelu(string imeLabele, int brLinije);
  bool obradiSekciju(string imeSekcije, int brLinije);

  void napraviRelokacioniZapis(UlazUTabeluObracanjaUnapred);
  int napisiInstrukciju(kodOperacije opKod, int a, int b, int c, int d);
  void napraviZakrpu(yytokentype opKod, Operand* operand, int gpr1, int gpr2, int csr);
  bool proveriNedefinisaneSimbole();
  void izbaciNepotrebneSimboleIzBazena();
  void popuniBazeneLiterala();
  void napraviBinarniFajl();

  vector<string>* nedefinisaniSimboli;

  ofstream fajl;
  const char* imeIzlaznogFajla;
  const char* imeUlaznogFajla;
  FILE* ulazniFajl;
  

public:
  static Asembler* getInstance(){
    if(!instanca){
      instanca = new Asembler();
    }
    return instanca;
  }

  void init(const char* inFajl, const char* outFajl){
    this->imeUlaznogFajla = inFajl;
    this->imeIzlaznogFajla = outFajl;
    string ime = outFajl;
    ime = ime.substr(0, ime.size()-2) + "Dump.txt";
    fajl.open(ime); // txt fajl kako bi korisnik imao izlaz asemblera u citljivom obliku
  }

  bool prolaz();
  void ispisiTabeluSimbola();
  void ispisiTabeluSekcija();
  void ispisiKodSekcija();
  void ispisiRelokacioneZapise();

};


#endif