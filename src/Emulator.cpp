#include "../inc/Emulator.hpp"

Emulator* Emulator::instanca = nullptr;

unsigned long Emulator::VELICINA_MEMORIJE = 1UL << 32;
unsigned long Emulator::POCETNA_ADRESA = 0x040000000;
unsigned long Emulator::ADRESA_MEMORIJSKIH_MAPIRANIH_REGISTARA = 0x0FFFFFF00;
unsigned long Emulator::INSTRUKCIJSKA_GRESKA = 0x1;
unsigned long Emulator::PREKID_TAJMER = 0x2;
unsigned long Emulator::PREKID_TERMINAL = 0x3;
unsigned long Emulator::SOFTVERSKI_PREKID = 0x4;
unsigned long Emulator::TERM_IN = 0x0FFFFFF04;
unsigned long Emulator::TERM_OUT = 0x0FFFFFF00;

Emulator::Emulator() : jelRadi(false), prekidTerminala(false), gp_registri(16, 0), cs_registri(3,0) {}

void Emulator::procitajUlaz(){

  ifstream binFajl(ulazniFajl, ios::binary);
  if(!binFajl){
    cout << "GRESKA: fajl: " << ulazniFajl << " ne postoji\n";
    exit(-1);
  }

  int brSekcija;
  binFajl.read((char*)&brSekcija, sizeof(int));
  for(int i = 0; i < brSekcija; i++){
    Sekcija nova;
    binFajl.read((char*)&nova.pocetnaAdresa, sizeof(unsigned int));
    binFajl.read((char*)&nova.velicina, sizeof(int));
    string kod;
    kod.resize(nova.velicina);
    binFajl.read((char*)kod.c_str(), nova.velicina);
    nova.kod = new stringstream();
    nova.kod->write((char*)kod.c_str(), nova.velicina);

    sekcije.push_back(nova);
  }
}

void Emulator::ispisiUlaz(){
  ofstream fajl;
  fajl.open("provera.txt");
  fajl << "--------------------Ulazni kod za izvrsavanje---------------------\n";
  for(Sekcija s : sekcije){
      fajl << "------------Pocetna adresa: " << hex << s.pocetnaAdresa <<  "---------------" << endl;
      fajl << dec;
      if(s.velicina == 0){
        fajl << "Sekcija nema podataka" << endl << endl;
      }
      else{
        fajl << "Velicina sekcije[hex]: " << hex << s.velicina << dec << endl;
        int brRedova = (s.velicina + 7) / 8 ;

        for(int i = 0; i < brRedova; i++){
          fajl << hex << setfill('0') << setw(4) << (0xffff & i*8) << ": ";
          for(int j = i*8; j < (i+1)*8; j++){
            char c;
            if(j < s.velicina){
              s.kod->read(&c, sizeof(char));
            }
            else{
              c = 0;
            }
            fajl << hex << setfill('0') << setw(2) << (0xff & c) << ' ';
          }
          fajl << endl;
        }
      }
      fajl << endl;
    }
    fajl.close();
}

void Emulator::mapirajMemoriju(){
  memorija = mmap(nullptr, VELICINA_MEMORIJE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_NORESERVE | MAP_PRIVATE, -1, 0 );
  if(memorija == MAP_FAILED){
    cout << "Neuspeh pri mapiranju memorije.\n";
    exit(-1);
  }

  for(Sekcija s : sekcije){
    char* adresa = static_cast<char*>(memorija) + s.pocetnaAdresa;
    memcpy(adresa, s.kod->str().c_str(), s.velicina);
  }
}

void Emulator::ispisiMapiranuMemoriju(){
  ofstream fajl;
  fajl.open("mapiranaMemorija.txt");
  fajl << "--------------------Mapirana memorija---------------------\n";
  sort(sekcije.begin(), sekcije.end(), [](Sekcija s1, Sekcija s2){
    return s1.pocetnaAdresa < s2.pocetnaAdresa;
  });
  for(Sekcija s : sekcije){
      fajl << "------------Pocetna adresa: " << hex << s.pocetnaAdresa <<  "---------------" << endl;
      fajl << dec;
      if(s.velicina == 0){
        fajl << "Sekcija nema podataka" << endl << endl;
      }
      else{
        fajl << "Velicina sekcije[hex]: " << hex << s.velicina << dec << endl;
        int brRedova = (s.velicina + 7) / 8 ;

        for(int i = 0; i < brRedova; i++){
          fajl << hex << setfill('0') << setw(4) << (0xffffffff & i*8+s.pocetnaAdresa) << ": ";
          for(int j = i*8; j < (i+1)*8; j++){
            char c;
            if(j < s.velicina){
              c = procitajBajt(s.pocetnaAdresa + j);
              s.kod->read(&c, sizeof(char));
            }
            else{
              c = 0;
            }
            fajl << hex << setfill('0') << setw(2) << (0xff & c) << ' ';
          }
          fajl << endl;
        }
      }
      fajl << endl;
    }
    fajl.close();
}


void Emulator::prepoznavanje_i_izvrsavanje_instrukcije(){
  kodOperacije opKod;
  int a, b, c, d; // Instrukcija: opKod|ab|cd|dd

  unsigned int instrukcija = procitajInt(rpc);
  rpc += 4; // Ovo radim odmah da bi u toku izvrsavanja instrukcije pc pokazivao na adresu naredne instrukcije
  opKod = (kodOperacije)(instrukcija >> 24);
  a = (instrukcija >> 20) & 0x0f;
  b = (instrukcija >> 16) & 0x0f;
  c = (instrukcija >> 12) & 0x0f;
  d = instrukcija & 0x0fff;
  
  if(d & 0x800){ // ovo znaci da je pomeraj negativan pa uradimo drugi komplement
    d = -1 * ((~d & 0xfff) + 1);
  }
  if(a < 0 || a > 15 || b < 0 || b > 15 || c < 0 || c > 15){
    // Nevalidan registar
    cout << "GRESKA: Nevalidan registar za instrukciju operacionog koda: 0x" << hex <<  opKod << endl;
    napraviPrekid(INSTRUKCIJSKA_GRESKA);
    return;
  }
  
  if(opKod == HALT){
    jelRadi = false;
  }
  else if(opKod == INT){
    push(cs_registri[status]);
    push(rpc);
    rpc = cs_registri[handler]; // Skocicemo na adresu prekidne rutine 
    cs_registri[cause] = SOFTVERSKI_PREKID; 
    cs_registri[status] &= ~0x01; // Omogucimo softverski prekid
  }
  else if(opKod == XCHG){
    unsigned long tmp = gp_registri[b];
    gp_registri[b] = gp_registri[c];
    gp_registri[c] = tmp;
  }
  else if(opKod == ADD){
    gp_registri[a] = gp_registri[b] + gp_registri[c];
  }
  else if(opKod == SUB){
    gp_registri[a] = gp_registri[b] - gp_registri[c];
  }
  else if(opKod == MUL){
    
    gp_registri[a] = gp_registri[b] * gp_registri[c];
    
  }
  else if(opKod == DIV){
    if(gp_registri[c] == 0){
      cout << "GRESKA: Deljenje nulom\n";
      napraviPrekid(INSTRUKCIJSKA_GRESKA);
      return;
    }
    gp_registri[a] = gp_registri[b] / gp_registri[c];
  }
  else if(opKod == NOT){
    gp_registri[a] = ~gp_registri[b];
  }
  else if(opKod == AND){
    gp_registri[a] = gp_registri[b] & gp_registri[c];
  }
  else if(opKod == OR){
    gp_registri[a] = gp_registri[b] | gp_registri[c];
  }
  else if(opKod == XOR){
    gp_registri[a] = gp_registri[b] ^ gp_registri[c];
  }
  else if(opKod == SHL){
    gp_registri[a] = gp_registri[b] << gp_registri[c];
  }
  else if(opKod == SHR){
    gp_registri[a] = gp_registri[b] >> gp_registri[c];
  }
  else if(opKod == CSRRD){
    gp_registri[a] = cs_registri[b];
  }
  else if(opKod == CSRWR){
    cs_registri[a] = gp_registri[b];
  }
  else if(opKod == PUSH){
    gp_registri[a] += d;
    upisiInt(gp_registri[a], gp_registri[c]);
  }
  else if(opKod == POP){
    gp_registri[a] = procitajInt(gp_registri[b]);
    gp_registri[b] += d;
  }
  else if(opKod == JMP_MEM){
    rpc = procitajInt(gp_registri[a] + d);
  }
  else if(opKod == JMP_IMM){
    rpc = gp_registri[a] + d;
  }
  else if(opKod == BGT_MEM){
    if(gp_registri[b] > gp_registri[c]){
      rpc = procitajInt(gp_registri[a] + d);
    }
  }
  else if(opKod == BGT_IMM){
    if(gp_registri[b] > gp_registri[c]){
      rpc = gp_registri[a] + d;
    }
  }
  else if(opKod == BNE_MEM){
    if(gp_registri[b] != gp_registri[c]){
      rpc = procitajInt(gp_registri[a] + d);
    }
  }
  else if(opKod == BNE_IMM){
    if(gp_registri[b] != gp_registri[c]){
      rpc = gp_registri[a] + d;
    }
  }
  else if(opKod == BEQ_MEM){
    if(gp_registri[b] == gp_registri[c]){
      rpc = procitajInt(gp_registri[a] + d);
    }
  }
  else if(opKod == BEQ_IMM){
    if(gp_registri[b] == gp_registri[c]){
      rpc = gp_registri[a] + d;
    }
  }
  else if(opKod == CALL_MEM){
    push(rpc);
    rpc = procitajInt(gp_registri[a] + d);
  }
  else if(opKod == CALL_IMM){
    push(rpc);
    rpc = gp_registri[a] + d;
  }
  else if(opKod == LD_MEM_REG){  
    gp_registri[a] = procitajInt(gp_registri[b] + d);
  }
  else if(opKod == LD_REG){
    gp_registri[a] = gp_registri[b] + d;
  }
  else if(opKod == ST_MEM){
    upisiInt(gp_registri[a] + gp_registri[b] + d, gp_registri[c]);
  }
  else if(opKod == ST_MEM_MEM){
    upisiInt(procitajInt(gp_registri[a] + gp_registri[b] + d), gp_registri[c]);
  }
  else if(opKod == CSRWR_MEM){
    cs_registri[a] = procitajInt(gp_registri[b] + gp_registri[c] + d);
  }
  else{
    cout << "Nepoznata instrukcija sa kodom operacije: 0x" << hex << opKod << endl;
  }

}


void Emulator::emuliraj(){

  procitajUlaz();
  mapirajMemoriju();
  ispisiMapiranuMemoriju(); // Ovo je samo za proveru

  rpc = POCETNA_ADRESA;
  rsp = ADRESA_MEMORIJSKIH_MAPIRANIH_REGISTARA;

  // Dozvolimo sve prekide
  cs_registri[status] &= ~0x4; // Dozvolimo softverske prekide
  cs_registri[status] &= ~0x2; // Dozvolimo prekide od terminala
  cs_registri[status] &= ~0x1; // Dozvolimo prekide od tajmera

  napraviTerminal();

  jelRadi = true;

  while(jelRadi == true){
    prepoznavanje_i_izvrsavanje_instrukcije();

    citajSaStandardnogUlaza();
    if(prekidTerminala == true){
      napraviPrekid(PREKID_TERMINAL);
      prekidTerminala = false;
    }
    
  }

  restartujTerminal();

  ispisiRegistre();
  ispisiMapiranuMemoriju();
  // Da oslobodimo mapiranu memoriju.
  munmap(memorija, VELICINA_MEMORIJE);
}


char Emulator::procitajBajt(unsigned int adresa){
  char* bajt = static_cast<char*>((static_cast<char*>(memorija) + adresa));
  return *bajt;
}

unsigned int Emulator::procitajInt(unsigned int adresa){
  unsigned int *podatak = reinterpret_cast<unsigned int*>(static_cast<char*>(memorija) + adresa);
  return *podatak;
}

void Emulator::upisiBajt(unsigned int adresa, char podatak){
  // Ovde treba dodati nesto za terminal
  if(adresa == TERM_OUT){
    cout << podatak;
  }
  char* bajt = static_cast<char*>((static_cast<char*>(memorija) + adresa));
  *bajt = podatak;
}

void Emulator::upisiInt(unsigned int adresa, int podatak){
  // Ovde treba dodati nesto za terminal
  if(adresa == TERM_OUT){
    cout << (char)podatak << flush;
  }
  int *mesto = reinterpret_cast<int*>(static_cast<char*>(memorija) + adresa);
  *mesto = podatak;
}

void Emulator::push(int podatak){
  rsp -= 4;
  upisiInt(rsp, podatak);
}

int Emulator::pop(){
  int podatak = procitajInt(rsp);
  rsp += 4;
  return podatak;
}

void Emulator::napraviPrekid(unsigned long uzrok){
  // Pusujemo status ,zatim pc
  push(cs_registri[status]);
  push(rpc);

  cs_registri[cause] = uzrok;
  // skocimo na prekidnu rutinu
  rpc = cs_registri[handler]; // Ocekujemo da je programer u handler upisao adresu prekidne rutine

  cs_registri[status] |= 0x4; // Maskiramo softverske prekide
  cs_registri[status] |= 0x2; // Maskiramo prekide od terminala
  cs_registri[status] |= 0x1; // Maskiramo prekide od tajmera
}

void Emulator::ispisiRegistre(){
  cout << endl;
  cout << "----------------------------------------------------------------------" << endl;
  cout << "Emulated processor executed halt instruction.\n";
  cout << "Emulated processor state:\n";
  for(int i = 0; i < 16; i++){
    string reg = "r" + to_string(i);
    cout << "\t" << setw(3) << setfill(' ') << reg << "=0x" << setw(8) << hex << setfill('0') << gp_registri[i];
    
    if(i % 4 == 3) cout <<  endl;
  }
  cout << endl;
}

struct termios staroPodesavanje; // Ovde pamtimo staro podesavanje terminala
struct termios novoPodesavanje;  // Ovde cemo konfigurisati novo podesavanje terminala

void Emulator::napraviTerminal(){
  tcgetattr(STDIN_FILENO, &staroPodesavanje);
  
  novoPodesavanje = staroPodesavanje;

  novoPodesavanje.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN);
  novoPodesavanje.c_cc[VTIME] = 0;
  novoPodesavanje.c_cc[VMIN] = 0;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &novoPodesavanje);

}

void Emulator::restartujTerminal(){
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &staroPodesavanje);
}

void Emulator::citajSaStandardnogUlaza(){
  char karakter;
  if(read(STDIN_FILENO, &karakter, 1) == 1){
    upisiBajt(TERM_IN, karakter);
    prekidTerminala = true;
  }
}