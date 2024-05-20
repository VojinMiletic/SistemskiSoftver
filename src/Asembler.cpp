#include "../inc/Asembler.hpp"
#include "../inc/helper.hpp"

Asembler* Asembler::instanca = nullptr;

Asembler::Asembler(): LC(0), redniBrojSimbola(0), redniBrojSekcije(0),trenutnaSekcija(""){
  // postavimo prvo UND sekciju
  UlazUTabeluSekcija sekcija;
  sekcija.idSekcije = redniBrojSekcije++;
  sekcija.imeSekcije = "UND";
  sekcija.velicina = 0;
  sekcija.tabelaObracanjaUnapred = nullptr;
  tabelaSekcija["UND"] = sekcija;

  kodSekcije["UND"] = stringstream();

  // Postavimo prvi ulaz u tabelu simbola koji predstavlja nedefinisani simbol
  UlazUTabeluSimbola noviUlaz;
  noviUlaz.idSimbola = redniBrojSimbola++;
  noviUlaz.definisan = true;
  noviUlaz.imeSimbola = "UND";
  noviUlaz.isExtern = false;
  noviUlaz.isGlobal = false;
  noviUlaz.sekcija = "UND";
  noviUlaz.vrednost = 0;
  noviUlaz.tabelaObracanjaUnapred = nullptr;
  tabelaSimbola["UND"] = noviUlaz;
}

bool Asembler::prolaz(){
  bool sveOk = true;
  for(int i = 0; i < elementi_help->size(); i++){
    sveOk = elementi_help->at(i)->prolaz();
    if(sveOk == false){
      return false;
    }
  }
  return true;
}

bool Asembler::obradiLabelu(string imeLabele, int brLinije){
  
  if(trenutnaSekcija == ""){
    greske[brLinije] = "GRESKA: Definisanje labele van sekcije na liniji: " + brLinije;
    cout << "GRESKA: Definisanje labele van sekcije na liniji: " << brLinije << endl;
    return false;
  }
  map<string, UlazUTabeluSimbola>::iterator iter = tabelaSimbola.find(imeLabele);
  // Simbol se nalayi u tabeli
  if(iter != tabelaSimbola.end()){
    // Ako je simbol vec definisan baci gresku
    if(iter->second.definisan == true){
      greske[brLinije] = "GRESKA: Simbol je vec definisan";
      cout << "GRESKA: Simbol je vec definisan, linija: " << brLinije << endl;
      return false;
    }
    // Ako je simbol eksterni baci gresku
    if(iter->second.isExtern == true){
      greske[brLinije] = "GRESKA: Definicija eksternog simbola";
      cout << "GRESKA: Definicija eksternog simbola, linija: " << brLinije << endl;
      return false;
    }
    iter->second.definisan = true;
    iter->second.sekcija = trenutnaSekcija;
    iter->second.vrednost = LC;
  }
  else{ // Simbol se ne nalazi u tabeli simbola, dodajemo ga
    UlazUTabeluSimbola novi;
    novi.definisan = true;
    novi.isExtern = false;
    novi.isGlobal = false; // Za sad, mozda se proglasi kao globalan
    novi.idSimbola = redniBrojSimbola++;
    novi.imeSimbola = imeLabele;
    novi.sekcija = trenutnaSekcija;
    novi.vrednost = LC;
    novi.tabelaObracanjaUnapred = new vector<UlazUTabeluObracanjaUnapred>();
    
    

    tabelaSimbola[imeLabele] = novi;
  }
  
  return true;
}

bool Asembler::obradiGlobal(string imeSimbola, int brLinije){
  
  map<string,UlazUTabeluSimbola>::iterator iter = tabelaSimbola.find(imeSimbola);

  if(iter != tabelaSimbola.end()){
    // Simbol se nalazi u tabeli
    if(iter->second.isExtern == true){
      greske[brLinije] = "GRESKA: Proglasili ste eksterni simbol globalnim.";
      cout << "GRESKA: Proglasili ste eksterni simbol globalnim, linija: " << brLinije << endl;
      return false; 
    }
    iter->second.isGlobal = true;
  }
  else{
    // Simbol se ne nalazi u tabeli simbola
    UlazUTabeluSimbola novi;
    novi.definisan = false;
    novi.isExtern = false;
    novi.isGlobal = true;
    novi.idSimbola = redniBrojSimbola++;
    novi.imeSimbola = imeSimbola;
    novi.sekcija = "UND";
    novi.vrednost = 0; // Simbol nije definisan pa mu ne znamo ni sekciju ni vrednostu
    novi.tabelaObracanjaUnapred = new vector<UlazUTabeluObracanjaUnapred>();
    

    tabelaSimbola[imeSimbola] = novi;
  }
  
  return true;
}


bool Asembler::obradiExtern(string imeSimbola, int brLinije){
  
  map<string,UlazUTabeluSimbola>::iterator iter = tabelaSimbola.find(imeSimbola);
  
  if(iter != tabelaSimbola.end()){
    if(iter->second.definisan == true){
      greske[brLinije] = "GRESKA: Simbol je definisan a proglasavate ga eksternim.";
      cout << "GRESKA: Simbol je definisan a proglasavate ga eksternim, linija: " << brLinije << endl;
      return false;
    }
    if(iter->second.isGlobal == true){
      greske[brLinije] = "GRESKA: Simbol je globalan a proglasavate ga eksternim.";
      cout << "GRESKA: Simbol je globalan a proglasavate ga eksternim, linija: " << brLinije << endl;
      return false;
    }
    iter->second.isExtern = true;
    iter->second.sekcija = "UND";
    iter->second.vrednost = 0; // Ne znamo sta je jer je simbol eksterni
  }
  else{
    UlazUTabeluSimbola novi;
    novi.definisan = false;
    novi.isExtern = true;
    novi.isGlobal = false;
    novi.idSimbola = redniBrojSimbola++;
    novi.imeSimbola = imeSimbola;
    novi.sekcija = "UND";
    novi.vrednost = 0; // Simbol nije definisan pa mu ne znamo ni sekciju ni vrednostu
    novi.tabelaObracanjaUnapred = new vector<UlazUTabeluObracanjaUnapred>();
    

    tabelaSimbola[imeSimbola] = novi;
  }
  
  return true;
}

bool Asembler::obradiSekciju(string imeSekcije, int brLinije){
  
  map<string,UlazUTabeluSekcija>::iterator iter = tabelaSekcija.find(imeSekcije);
  if(iter != tabelaSekcija.end()){
    // Sekcija je vec deklarisana
    greske[brLinije] = "GRESKA: Sekcija je vec definisana";
    cout << "GRESKA: Sekcija je vec definisana, linija: " << brLinije << endl;
    return false;
  }

  if(trenutnaSekcija != ""){
    tabelaSekcija[trenutnaSekcija].velicina = LC;
  }
  LC = 0;
  trenutnaSekcija = imeSekcije;

  UlazUTabeluSekcija novaSekcija;
  novaSekcija.idSekcije = redniBrojSekcije++;
  novaSekcija.imeSekcije = imeSekcije;
  novaSekcija.velicina = 0;
  novaSekcija.tabelaObracanjaUnapred = new vector<UlazUTabeluObracanjaUnapred>();
  novaSekcija.redniBrojKonstante = 0;
  tabelaSekcija[imeSekcije] = novaSekcija;

  UlazUTabeluSimbola novi;
  novi.definisan = true;
  novi.idSimbola = redniBrojSimbola++;
  novi.imeSimbola = imeSekcije;
  novi.isExtern = false;
  novi.isGlobal = false;
  novi.vrednost = 0;
  novi.sekcija = imeSekcije;
  novi.tabelaObracanjaUnapred = new vector<UlazUTabeluObracanjaUnapred>();
  
  tabelaSimbola[imeSekcije] = novi;
  
  return true;
}

bool Asembler::obradiSkip(int vrednostLiterala, int brLinije){
  
  
  if(trenutnaSekcija == ""){
    greske[brLinije] = "GRESKA: Definisanje skip direktive izvan sekcije.";
    cout << "GRESKA: Definisanje skip direktive izvan sekcije, linija: " << brLinije << endl;
    return false;
  }

  for(int i = 0; i < vrednostLiterala; ++i){
    char kod = 0;
    kodSekcije[trenutnaSekcija].write(&kod, sizeof(char));
  }
  LC += vrednostLiterala;
  
  return true;
}

bool Asembler::obradiWord(pair<bool, Unija> argument, int brLinije){

  if(trenutnaSekcija == ""){
    greske[brLinije] = "GRESKA: direktiva .word je izvan sekcije.";
    cout << "GRESKA: direktiva .word je izvan sekcije, linija: " << brLinije << endl;
    return false;
  }

  // Ako je u pitanju literal...
  if(argument.first == true){
    int num = argument.second.broj;
    kodSekcije[trenutnaSekcija].write((char*)&num, sizeof(int));
    LC += 4;
    return true;
  }
  // Ako je u pitanju simbol proverimo u tabeli simbola prvo.
  map<string,UlazUTabeluSimbola>::iterator iter = tabelaSimbola.find(argument.second.simbol);
  // Ako je simbol u tabeli simbola...
  if(iter != tabelaSimbola.end()){
   // zapravo ne radimo nista, sve jedno moramo da ostavimo informaciju o relokacionom zapisu
  }
  else{
    UlazUTabeluSimbola noviSimbol;
    noviSimbol.definisan = false;
    noviSimbol.idSimbola = redniBrojSimbola++;
    noviSimbol.imeSimbola = argument.second.simbol;
    noviSimbol.isExtern = false;
    noviSimbol.isGlobal = false;
    noviSimbol.sekcija = "UND";
    noviSimbol.vrednost = 0;
    noviSimbol.tabelaObracanjaUnapred = new vector<UlazUTabeluObracanjaUnapred>();
    
    tabelaSimbola[argument.second.simbol] = noviSimbol;
  }
  UlazUTabeluObracanjaUnapred novi;
  novi.instrukcija = TOKEN_WORD;
  novi.trenutnaSekcija = trenutnaSekcija;
  novi.trenutniLC = LC;
  novi.imeSimbola = argument.second.simbol;
  tabelaSimbola[argument.second.simbol].tabelaObracanjaUnapred->push_back(novi);
  
  
  int kod = 0;
  
  
  kodSekcije[trenutnaSekcija].write((char*)&kod, sizeof(int));
  LC += 4;
  return true;
}

bool Asembler::obradiAscii(string tekst, int brLinije){

  if(trenutnaSekcija == ""){
    greske[brLinije] = "GRESKA: Definisanje ascii direktive izvan sekcije.";
    cout << "GRESKA: Definisanje ascii direktive izvan sekcije, linija: " << brLinije << endl;
    return false;
  }  

  for(int i = 0; i < tekst.size(); i++){
    char c = tekst.at(i);
    kodSekcije[trenutnaSekcija].write(&c, sizeof(char));
  }
  char c = '\0';
  kodSekcije[trenutnaSekcija].write(&c, sizeof(char));

  LC += (tekst.size() + 1);

  return true;
}

bool Asembler::obradiEnd(int brLinije){

  // Ovo treba doraditi


  // Zavrsimo sa trenutnom sekcijom
  tabelaSekcija[trenutnaSekcija].velicina = LC;
  LC = 0;
  trenutnaSekcija = "";
 
  // Prodjemo kroz sve tabele obracanja unapred i generisemo relokacione zapise tamo gde to treba

  // Prodjemo kroz tabelu obracanja unapred u sekcijama
  for(map<string, UlazUTabeluSekcija>::iterator iter = tabelaSekcija.begin(); iter != tabelaSekcija.end(); iter++){
    if(iter->second.tabelaObracanjaUnapred == nullptr) continue;

    for(int i = 0; i < iter->second.tabelaObracanjaUnapred->size(); i++){
      UlazUTabeluObracanjaUnapred& ulaz = iter->second.tabelaObracanjaUnapred->at(i);
      if(ulaz.instrukcija == TOKEN_JMP){
        int d = tabelaSekcija[ulaz.trenutnaSekcija].velicina + 4 * ulaz.redBrSimbolaUBazenu - ulaz.trenutniLC - 4;
        int kodInstrukcije = napisiInstrukciju(JMP_MEM, 15, 0, 0, d);
        kodSekcije[ulaz.trenutnaSekcija].seekp(ulaz.trenutniLC); // Postavimo mesto gde pisemo
        kodSekcije[ulaz.trenutnaSekcija].write((char*)&kodInstrukcije, sizeof(int));
        kodSekcije[ulaz.trenutnaSekcija].seekp(0, std::ios_base::end); // Vratimo mesto za pisanje na kraj sekcije
      }
    }
  }


  vector<pair<string,UlazUTabeluSimbola>> ts(tabelaSimbola.begin(), tabelaSimbola.end());

  for(pair<string,UlazUTabeluSimbola> iter : ts){
    if(iter.second.tabelaObracanjaUnapred == nullptr){
      continue;
    }
    for(int i = 0; i < iter.second.tabelaObracanjaUnapred->size(); i++){
      UlazUTabeluObracanjaUnapred& ulaz = iter.second.tabelaObracanjaUnapred->at(i);
      yytokentype instrukcija = ulaz.instrukcija;
      if(instrukcija == TOKEN_WORD){ /*Ovo treba doraditi, dodati i ostale instrukcije za koje trebaju rel. zapisi*/
        napraviRelokacioniZapis(ulaz);
      }
      else if(instrukcija == TOKEN_JMP){
        int d = tabelaSekcija[ulaz.trenutnaSekcija].velicina + 4 * ulaz.redBrSimbolaUBazenu - ulaz.trenutniLC - 4;
        int kodInstrukcije = napisiInstrukciju(JMP_MEM, 15, 0, 0, d);
        kodSekcije[ulaz.trenutnaSekcija].seekp(ulaz.trenutniLC); // Postavimo mesto gde pisemo
        kodSekcije[ulaz.trenutnaSekcija].write((char*)&kodInstrukcije, sizeof(int));
        kodSekcije[ulaz.trenutnaSekcija].seekp(0, std::ios_base::end); // Vratimo mesto za pisanje na kraj sekcije
        if(iter.second.sekcija != ulaz.trenutnaSekcija){
          // Treba relokacioni zapis, on se odnosi na ulaz u bazenu literala
          ulaz.trenutniLC = tabelaSekcija[ulaz.trenutnaSekcija].velicina + 4 * ulaz.redBrSimbolaUBazenu;
          napraviRelokacioniZapis(ulaz);
        }
      }   
    }
  }

  // Upisujemo bazene u kod svake sekcije
  for(map<string, UlazUTabeluSekcija>::iterator iter = tabelaSekcija.begin(); iter != tabelaSekcija.end(); iter++){
    
    map<int,int> pomocna;
    for(map<int,UlazUTabeluLiterala>::iterator i = iter->second.tabelaLiterarnihKonstanti.begin(); 
      i != iter->second.tabelaLiterarnihKonstanti.end(); i++){
        pomocna[i->second.redniBroj] = i->first;
      }
    for(map<string,UlazUTabeluLiterala>::iterator i = iter->second.tabelaSimbolickihKonstanti.begin(); 
      i != iter->second.tabelaSimbolickihKonstanti.end(); i++){
        pomocna[i->second.redniBroj] = 0;
      }
    vector<pair<int,int>> vec(pomocna.begin(), pomocna.end());
    sort(vec.begin(), vec.end()); // Sortiramo vektor po redosledu literala u bazenu 

    for(int i = 0; i < vec.size(); i++){
      int value = vec.at(i).second;
      kodSekcije[iter->first].write((char*)&value, sizeof(int));
    }
    
    // Svakoj sekciji povecamo velicinu u skladu sa brojem literala u bazenu
    iter->second.velicina += 4 * iter->second.redniBrojKonstante;
  }


  ispisiTabeluSimbola();
  ispisiTabeluSekcija();
  ispisiKodSekcija();
  ispisiRelokacioneZapise();
  cout << "Program zavrsen.\n";
  exit(0);
  return true;
}


bool Asembler::obradiInstrukciju(yytokentype token, int brLinije, char* mnemonik, Operand* operand, int gpr1, int gpr2, int csr){

  kodOperacije oprKod;
  switch(token){
    case TOKEN_HALT:{
      int instr = napisiInstrukciju(HALT, 0, 0, 0, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_INT:{
      int instr = napisiInstrukciju(INT, 0, 0, 0, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_RET:{
      int instr = napisiInstrukciju(POP,15, 14, 0, 4); // PC = MEM[SP]; SP += 4
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int)); 
      break;
    }
    case TOKEN_IRET:{
      int instr = napisiInstrukciju(POP,15, 14, 0, 4); // PC = MEM[SP]; SP += 4
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int)); 
      instr = napisiInstrukciju(CSRWR_MEM_POSTINC, 0, 14, 0, 4); // CSR(0)(to je status) = MEM[SP]; SP += 4
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      
      LC += 4;
      break; 
    }
    case TOKEN_ADD: {
      int instr = napisiInstrukciju(ADD, gpr2, gpr2, gpr1, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_SUB: {
      int instr = napisiInstrukciju(SUB, gpr2, gpr2, gpr1, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_MUL: {
      int instr = napisiInstrukciju(MUL, gpr2, gpr2, gpr1, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_DIV: {
      int instr = napisiInstrukciju(DIV, gpr2, gpr2, gpr1, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_SHL: {
      int instr = napisiInstrukciju(SHL, gpr2, gpr2, gpr1, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_SHR: {
      int instr = napisiInstrukciju(SHR, gpr2, gpr2, gpr1, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_AND: {
      int instr = napisiInstrukciju(AND, gpr2, gpr2, gpr1, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_OR: {
      int instr = napisiInstrukciju(OR, gpr2, gpr2, gpr1, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_XOR: {
      int instr = napisiInstrukciju(XOR, gpr2, gpr2, gpr1, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_NOT: {
      int instr = napisiInstrukciju(NOT, gpr1, gpr1, 0, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_XCHG: {
      int instr = napisiInstrukciju(XCHG, 0, gpr2, gpr2, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_POP: {
      int instr = napisiInstrukciju(POP, gpr1, 14, 0, 4); // gpr1 = MEM[SP]; SP += 4
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_PUSH: {
      int instr = napisiInstrukciju(PUSH, 14, 0, gpr1 ,-4); // SP -= 4; MEM[SP] = gpr1
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_CSRRD: {
      int instr = napisiInstrukciju(CSRRD, gpr1, csr, 0, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_CSRWR: {
      int instr = napisiInstrukciju(CSRWR, csr, gpr1, 0, 0);
      kodSekcije[trenutnaSekcija].write((char*)&instr, sizeof(int));
      break;
    }
    case TOKEN_JMP: {
      izvediSkok(TOKEN_JMP,operand ,gpr1, gpr2);
    }
  }

  LC += 4; 
  return true;
}



void Asembler::ispisiTabeluSimbola(){
  fajl.open("simboli.txt");
  fajl << "-------------------------------------Tabela simbola--------------------------------------" << endl;
  fajl << "Redni broj\t" << "Ime simbola\t" << "Vrednost\t" << "Tip\t" << "Sekcija\t" << "ID sekcije\t" << "Vezivanje\t" << "Definisan\n";
  vector<pair<string,UlazUTabeluSimbola>> ts(tabelaSimbola.begin(), tabelaSimbola.end());
  // Sortiramo simbole po njihovom id-ju
  sort(ts.begin(), ts.end(), [](pair<string, UlazUTabeluSimbola> u1, pair<string, UlazUTabeluSimbola> u2){
    return u1.second.idSimbola < u2.second.idSimbola;
  });
  for(pair<string,UlazUTabeluSimbola> iter : ts){
    fajl << setw(10) << iter.second.idSimbola << '\t';
    fajl << setw(11) << iter.first << '\t';
    fajl << setw(8) << iter.second.vrednost << '\t';
    fajl << setw(6) << (iter.second.sekcija == iter.second.imeSimbola ? "SCTN" : "NOTYPE") << '\t';
    fajl << setw(7) << iter.second.sekcija << '\t';
    fajl << setw(9) <<  tabelaSimbola[iter.second.sekcija].idSimbola << '\t';
    fajl << setw(9) << ((iter.second.isGlobal == true || iter.second.isExtern == true) ? "GLOB" : "LOC" )<< '\t';
    fajl << setw(9) << (iter.second.definisan == true ? "DA" : "NE") << endl;
  }
  fajl << endl << endl;
  fajl.close();
}

void Asembler::ispisiTabeluSekcija(){
  fajl.open("simboli.txt", std::ios::app);
  fajl << "-------------Tabela sekcija-------------" << endl;
  fajl << "Redni broj\t" << "Ime sekcije\t" << "ID sekcije\t" << "Velicina\t" << endl;
  vector<pair<string,UlazUTabeluSekcija>> ts(tabelaSekcija.begin(), tabelaSekcija.end());
  for(pair<string,UlazUTabeluSekcija> iter : ts){
    fajl << setw(10) << iter.second.idSekcije << '\t';
    fajl << setw(11) << iter.second.imeSekcije << '\t';
    fajl << setw(9) << iter.second.idSekcije << '\t';
    fajl << setw(8) << iter.second.velicina << endl;
  }
  fajl << endl;
  fajl.close();
}

void Asembler::ispisiKodSekcija(){
  fajl.open("simboli.txt", std::ios::app);
  fajl << "------------------------------Sadrzaj sekcija----------------------------------------- \n";
  
  for(map<string, UlazUTabeluSekcija>::iterator iter = tabelaSekcija.begin(); iter != tabelaSekcija.end(); iter++){
    fajl << "------------Sekcija: " << iter->first <<  "---------------" << endl;
    fajl << dec;
    if(iter->second.velicina == 0){
      fajl << "Sekcija nema podataka" << endl << endl;
    }
    else{
      fajl << "Velicina sekcije: " << iter->second.velicina << endl;
      int brRedova = (iter->second.velicina + 7) / 8 ;

      for(int i = 0; i < brRedova; i++){
        fajl << hex << setfill('0') << setw(4) << (0xffff & i*8) << ": ";
        for(int j = i*8; j < (i+1)*8; j++){
          char c;
          if(j < iter->second.velicina){
            kodSekcije[iter->first].read(&c, sizeof(char));
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
  fajl << endl << endl << endl;
  fajl.close();
}

void Asembler::napraviRelokacioniZapis(UlazUTabeluObracanjaUnapred ulaz){
  // Potrebno je doraditi
  map<string, UlazUTabeluSimbola>::iterator iter = tabelaSimbola.find(ulaz.imeSimbola);
  if(iter != tabelaSimbola.end()){
    UlazURelokacionuTabelu novi;
    novi.offset = ulaz.trenutniLC;
    novi.simbol = (iter->second.isGlobal || iter->second.isExtern) ? ulaz.imeSimbola : iter->second.sekcija;
    novi.redBrSimbola = (iter->second.isGlobal || iter->second.isExtern) ? iter->second.idSimbola : tabelaSimbola[iter->second.sekcija].idSimbola ;
    novi.addend = (iter->second.isGlobal || iter->second.isExtern) ? 0 : iter->second.vrednost;
    novi.tip = "R_TYPE"; // (ulaz.instrukcija == TOKEN_WORD /*Ovde treba doraditi*/) ? "R_TYPE" : "PC_32_TYPE";
    tabelaRelokacija[ulaz.trenutnaSekcija].push_back(novi);
  }
  else{
    cout << "GRESKA: Imate nedefinisan simbol!" << endl;
  }
}

void Asembler::ispisiRelokacioneZapise(){
  
  fajl.open("simboli.txt", std::ios::app);
  fajl << "--------------------------------Relokacioni zapisi----------------------------------" << endl;

  for(map<string, UlazUTabeluSekcija>::iterator iter = tabelaSekcija.begin(); iter != tabelaSekcija.end(); iter++){
    
    fajl << "Tabela realokacija sekcije: " << iter->first << endl;
    
    map<string, vector<UlazURelokacionuTabelu>>::iterator i2 = tabelaRelokacija.find(iter->first);
    
    if(i2 == tabelaRelokacija.end()){
      fajl << "Za ovu sekciju ne postoje relokacioni zapisi\n\n";
    }
    else{
      fajl << "Offset\t" << "Ime simbola\t" << "ID simbola\t" << "Tip\t" << "Addend" << endl;
      for(int i = 0; i < i2->second.size(); i++){
        fajl << hex << setfill('0') << setw(8) << i2->second.at(i).offset << '\t';
        
        fajl << setw(2)<< setfill(' ') << i2->second.at(i).simbol << '\t';
        fajl << setw(9) << dec << i2->second.at(i).redBrSimbola << '\t';
        fajl << setw(3) << i2->second.at(i).tip << '\t';
        fajl << hex << setfill('0') << setw(8) << i2->second.at(i).addend << endl << endl;
      }
    }
  }

  fajl << endl << endl;
  fajl.close();
}

int Asembler:: napisiInstrukciju(kodOperacije opKod, int a, int b, int c, int d){
  int prvi = opKod << 24;
  int drugi = (((a << 4) & 0xf0 ) | (b & 0x0f) ) << 16;
  int treci = (((c << 4) & 0xf0) | ((d >> 8) & 0xf0)) << 8;
  int cetvrti = d & 0xff;
  int instrukcija = prvi | drugi | treci | cetvrti;
  return instrukcija;
}

void Asembler::izvediSkok(yytokentype instrukcija, Operand* operand, int gpr1, int gpr2){

  if(operand->simbol){

    map<string, UlazUTabeluSimbola>::iterator iter = tabelaSimbola.find(operand->simbol);
    if(iter == tabelaSimbola.end()){
      // Simbol se ne nalazi u tabeli simbola, dodajemo ga
      UlazUTabeluSimbola novi;
      novi.idSimbola = redniBrojSimbola++;
      novi.imeSimbola = operand->simbol;
      novi.definisan = false;
      novi.isExtern = false;
      novi.isGlobal = false;
      novi.sekcija = "UND";
      novi.vrednost = 0;
      novi.tabelaObracanjaUnapred = new vector<UlazUTabeluObracanjaUnapred>();
      
      tabelaSimbola[operand->simbol] = novi;
    }
    // Ovde sam siguran da se simbol nalazi u tabeli

    map<string,UlazUTabeluLiterala>::iterator i = tabelaSekcija[trenutnaSekcija].tabelaSimbolickihKonstanti.find(operand->simbol);
    if(i == tabelaSekcija[trenutnaSekcija].tabelaSimbolickihKonstanti.end()){
      // Simbol se ne nalazi u tabeli literala 
      UlazUTabeluLiterala novi;
      novi.redniBroj = tabelaSekcija[trenutnaSekcija].redniBrojKonstante++;
      tabelaSekcija[trenutnaSekcija].tabelaSimbolickihKonstanti[operand->simbol] = novi;
    }

    UlazUTabeluObracanjaUnapred novi;
    novi.imeSimbola = operand->simbol;
    novi.instrukcija = instrukcija;
    novi.operand = operand;
    novi.trenutnaSekcija = trenutnaSekcija;
    novi.trenutniLC = LC;
    novi.redBrSimbolaUBazenu = tabelaSekcija[trenutnaSekcija].tabelaSimbolickihKonstanti[operand->simbol].redniBroj;

    tabelaSimbola[operand->simbol].tabelaObracanjaUnapred->push_back(novi);

    int c = 0;
    kodSekcije[trenutnaSekcija].write((char*)&c, sizeof(int)); // Ove nule ovde su kao plejsholder dok ne izvrsimo prepravku
  }
  else{ // U pitanju je literal
    map<int,UlazUTabeluLiterala>::iterator i = tabelaSekcija[trenutnaSekcija].tabelaLiterarnihKonstanti.find(operand->literal);
    if(i == tabelaSekcija[trenutnaSekcija].tabelaLiterarnihKonstanti.end()){
      // Literal se ne nalazi u tabeli literala 
      UlazUTabeluLiterala novi;
      novi.redniBroj = tabelaSekcija[trenutnaSekcija].redniBrojKonstante++;
      tabelaSekcija[trenutnaSekcija].tabelaLiterarnihKonstanti[operand->literal] = novi;
    }

    UlazUTabeluObracanjaUnapred novi;
    novi.imeSimbola = "Literal";
    novi.instrukcija = instrukcija;
    novi.operand = operand;
    novi.trenutnaSekcija = trenutnaSekcija;
    novi.trenutniLC = LC;
    novi.redBrSimbolaUBazenu = tabelaSekcija[trenutnaSekcija].tabelaLiterarnihKonstanti[operand->literal].redniBroj;

    tabelaSekcija[trenutnaSekcija].tabelaObracanjaUnapred->push_back(novi);

    int c = 0;
    kodSekcije[trenutnaSekcija].write((char*)&c, sizeof(int));
  }
}
