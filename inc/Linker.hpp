#include <iostream>
using namespace std;
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>


class Linker{

  string izlazniFajl;
  vector<string> ulazniFajlovi;
  bool isHex;
  bool isRelocatable;
  map<string, unsigned int> postavljeneSekcije; // kljuc je ime sekcije, podatak je adresa od koje treba da bude smestena

  struct UlazUTabeluSekcija{
    int idSekcije;
    string ime;
    int velicina;
    unsigned int adresaPocetka;
    
  };

  struct UlazUTabeluSimbola{
    int idSimbola;
    string ime;
    bool definisan;
    bool globalan;
    bool eksteran;
    int vrednost;
    int idSekcije;
    
  };

  struct UlazURelokacionuTabelu{
    int addend;
    int offset;
    string simbol;
    
  };

  struct SpojenaSekcija{
    int idSpojeneSekcije;
    int velicina;
    unsigned int adresaPocetka;
    map<string, unsigned int> sekcijeKojeJeCine; // kljuc je fajl u kome je sekcja a podatak je ofset sekcije unutar spojene sekcije, sve ove sekcije imaju isto ime
  };

  int redniBrojSpojeneSekcije; // Spojenim sekcijama dodeljujemo id po ovom broju

  map<string, SpojenaSekcija> spojeneSekcije; // kljuc je ime spojene sekcije

  // Sluzice nam da prihvatimo podatke iz binarnih fajlova
  map<string, map<int, UlazUTabeluSekcija>> tabeleSekcija; // kljuc je fajl, podatak je tabela sekcija tog fajla 
  map<string, map<string,UlazUTabeluSimbola>> tabeleSimbola; // kljuc je fajl, podatak je tabela simbola tog fajla
  map<string, map<string,vector<UlazURelokacionuTabelu>>> tabeleRelokacija; // kljuc je fajl, podatak je tabela relokacja tog fajla
  map<string, map<string, stringstream*>> kodovi; //  kljuc je fajl, podatak su kodovi sekcija tog fajla


  map<string, UlazUTabeluSimbola> krajnjaTabelaSimbola;
  map<int, UlazUTabeluSekcija> krajnjaTabelaSekcija;
  map<string, vector<UlazURelokacionuTabelu>> krajnjaTabeleRelokacija;
  map<string, stringstream*> krajnjaTabelaKodova ; 


  void dohvatiPodatke();
  
  void spojiSekcijeIstogImena();
  void proveriPreklapanje();
  void mapirajSekcije();

  void ispisiMapiraneSekcije();

  void spojiTabeleSimbola();
  void ispisiKrajnjuTabeluSimbola();

  void spojiRelokacioneTabele();
  void ispisiKrajnjuRelokacionuTabelu();

  void spojiKodoveSekcija();
  void ispisiKrajnjuTabeluKodova();

  void napraviKrajnjuTabeluSekcija();

  void razresiRelokacioneZapise();

  void napraviBinarniFajl();


  static Linker* instanca;
  ofstream fajl;
  Linker();
public:
  static Linker* getInstance(){
    if(!instanca){
      instanca = new Linker();
    }
    return instanca;
  }

  void init(string izlazniFajl, vector<string> ulazniFajlovi, map<string,unsigned int> postavljeneSekcije , bool isHex, bool isRelocatable){
    this->izlazniFajl = izlazniFajl;
    this->ulazniFajlovi = ulazniFajlovi;
    this->postavljeneSekcije = postavljeneSekcije;
    this->isHex = isHex;
    this->isRelocatable = isRelocatable;

    this->fajl.open(izlazniFajl + "DUMP.txt"); // Otvaramo tekstualni fajl za pisanje po njemu  
  }

  void linkuj();

};