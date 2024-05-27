#include <iostream>
using namespace std;
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/mman.h>
#include <string.h>


class Emulator{


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

  enum GPR{
    r0 = 0,
    r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15,
    sp = 14,
    pc = 15
  };
  enum CSR{
    status = 0,
    handler,
    cause
  };

  struct Sekcija{
    unsigned int pocetnaAdresa;
    int velicina;
    stringstream* kod;
  };

  vector<long> gp_registri;
  vector<long> cs_registri;
  long &rpc = gp_registri[15];
  long &rsp = gp_registri[14];

  bool jelRadi;

  static Emulator* instanca;
  Emulator();
  string ulazniFajl;
  vector<Sekcija> sekcije;

  void *memorija; // Ovde ce biti mapirana memorija naseg procesora 


  void procitajUlaz();
  void ispisiUlaz();

  void mapirajMemoriju();
  void ispisiMapiranuMemoriju();

  // Pomocne funkcije
  char procitajBajt(unsigned int adresa);
  unsigned int procitajInt(unsigned int adresa);
  void upisiBajt(unsigned int adresa, char podatak);
  void upisiInt(unsigned int adresa, int podatak);

  void prepoznavanje_i_izvrsavanje_instrukcije();

  void push(int podatak);
  int pop();

  void napraviPrekid(unsigned long uzrok);

  void ispisiRegistre();

public:
  static unsigned long VELICINA_MEMORIJE;
  static unsigned long POCETNA_ADRESA;
  static unsigned long ADRESA_MEMORIJSKIH_MAPIRANIH_REGISTARA;
  static unsigned long INSTRUKCIJSKA_GRESKA;
  static unsigned long PREKID_TAJMER;
  static unsigned long PREKID_TERMINAL;
  static unsigned long SOFTVERSKI_PREKID;


  static Emulator* getInstance(){
    if(!instanca){
      instanca = new Emulator();
    }
    return instanca;
  }

  void init(string ulazniFajl){
    this->ulazniFajl = ulazniFajl;
  }

  void emuliraj();

};