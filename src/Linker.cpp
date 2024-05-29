#include "../inc/Linker.hpp"

Linker* Linker::instanca = nullptr;

Linker::Linker(){
  this->redniBrojSpojeneSekcije = 0;
}

void Linker::dohvatiPodatke(){ // tabela sekcija, tabela simbola, relokacioni zapisi, kodovi sekcija

  for(string fajl : ulazniFajlovi){
    ifstream binFajl(fajl, ios::binary);
    if(!binFajl){
      cout << "GRESKA: fajl: " << fajl << " ne postoji\n";
      exit(-1);
    }
    // Restauriramo tabelu sekcija, kljuc, id, velicna
    int brUlaza;
    binFajl.read((char*)&brUlaza, sizeof(int));
    map<int, UlazUTabeluSekcija> tabelaSekcija;
    for(int i = 0; i < brUlaza; i++){
      UlazUTabeluSekcija podaci;
     
      int duzina;
      string ime;
      binFajl.read((char*)&duzina, sizeof(int));
      ime.resize(duzina); // Da napravimo prostora za ucitavanje imena
      binFajl.read((char*)ime.c_str(), duzina);

      binFajl.read((char*)(&podaci.idSekcije), sizeof(int));
      binFajl.read((char*)(&podaci.velicina), sizeof(int));

      podaci.ime = ime; // Dodao
      podaci.adresaPocetka = 0;
      
      tabelaSekcija[podaci.idSekcije] = podaci;

    }
    tabeleSekcija[fajl] = tabelaSekcija;

    // Restauriramo tabelu simbola
    binFajl.read((char*)&brUlaza, sizeof(int));
    map<string, UlazUTabeluSimbola> tabelaSimbola;
    for(int i = 0; i < brUlaza; i++){
      UlazUTabeluSimbola podaci;

      int duzina;
      string ime;
      binFajl.read((char*)&duzina, sizeof(int));
      ime.resize(duzina); // Da napravimo prostora za ucitavanje imena
      binFajl.read((char*)ime.c_str(), duzina);

      binFajl.read((char*)(&podaci.definisan), sizeof(podaci.definisan));
      binFajl.read((char*)(&podaci.globalan), sizeof(podaci.globalan));
      binFajl.read((char*)(&podaci.idSimbola), sizeof(int));
      binFajl.read((char*)(&podaci.eksteran), sizeof(podaci.eksteran));
      binFajl.read((char*)(&podaci.vrednost), sizeof(int));

      podaci.ime = ime; // Dodao
      binFajl.read((char*)(&podaci.idSekcije), sizeof(int));

      tabelaSimbola[ime] = podaci;
    }
    tabeleSimbola[fajl] = tabelaSimbola;

    // Restauracija relokacionih tabela
    map<string, vector<UlazURelokacionuTabelu>> relokacije;
    binFajl.read((char*)&brUlaza, sizeof(int));
    for(int i = 0; i < brUlaza; i++){
      vector<UlazURelokacionuTabelu> podaci;
      string ime;
      int duzina;
      binFajl.read((char*)(&duzina), sizeof(int));
      ime.resize(duzina);
      binFajl.read((char*)ime.c_str(), duzina);

      int brRedova;
      binFajl.read((char*)&brRedova, sizeof(int));
      for(int j = 0; j < brRedova; j++){
        UlazURelokacionuTabelu zapis;
        binFajl.read((char*)&zapis.addend, sizeof(int));
        binFajl.read((char*)&zapis.offset, sizeof(int));
        binFajl.read((char*)(&duzina), sizeof(int));
        zapis.simbol.resize(duzina);
        binFajl.read((char*)zapis.simbol.c_str(), duzina);
        podaci.push_back(zapis);
      }
      relokacije[ime] = podaci;
    }
    tabeleRelokacija[fajl] = relokacije;

    // Restauracija koda sekcija
    binFajl.read((char*)&brUlaza, sizeof(int));
    map<string, stringstream*> kodFajla;
    for(int i = 0; i < brUlaza; i++){
      string ime;
      int duzina;
      binFajl.read((char*)(&duzina), sizeof(int));
      ime.resize(duzina);
      binFajl.read((char*)ime.c_str(), duzina);

      string kod;
      binFajl.read((char*)(&duzina), sizeof(int));
      kod.resize(duzina);
      binFajl.read((char*)kod.c_str(), duzina);
      kodFajla[ime] = new stringstream();
      kodFajla[ime]->write((char*)kod.c_str(), duzina);
    }
    kodovi[fajl] = kodFajla;
  
    binFajl.close();
  }
}


// Svaka sekcija ce zavrsiti u tabeli spojenih sekcija

void Linker::spojiSekcijeIstogImena(){
  // Dodacemo UND sekciju u tabelu 
  
  SpojenaSekcija ulaz;
  ulaz.adresaPocetka = 0;
  ulaz.idSpojeneSekcije = redniBrojSpojeneSekcije++;
  ulaz.velicina = 0;
  spojeneSekcije["UND"] = ulaz;

  for(string f : ulazniFajlovi){
    for(map<int,UlazUTabeluSekcija>::iterator i = tabeleSekcija[f].begin(); i != tabeleSekcija[f].end(); i++){
      if(i->second.ime == "UND") continue;
      
      map<string, SpojenaSekcija>::iterator it = spojeneSekcije.find(i->second.ime);
      if(it == spojeneSekcije.end()){ // Ako se sekcija ne nelazi u tabeli spojenih sekcija, dodaj je
        SpojenaSekcija noviUlaz;
        noviUlaz.idSpojeneSekcije = redniBrojSpojeneSekcije++;
        noviUlaz.adresaPocetka = 0;
        noviUlaz.velicina = 0;
        spojeneSekcije[i->second.ime] = noviUlaz;
      }
      // Ovde sam siguran da se sekcija nalazi u tabeli spojenih sekcija
      spojeneSekcije[i->second.ime].sekcijeKojeJeCine[f] = spojeneSekcije[i->second.ime].velicina;
      spojeneSekcije[i->second.ime].velicina += i->second.velicina;
    }
  }  
}

void Linker::proveriPreklapanje(){
  
  for(map<string,unsigned int>::iterator i = postavljeneSekcije.begin(); i != postavljeneSekcije.end(); i++){
    for(map<string,unsigned int>::iterator j = postavljeneSekcije.begin(); j != postavljeneSekcije.end(); j++){
      if(i->first == j->first) continue; // Ne poredimo sekciju samu sa sobom
      // adr1-----------adr1+vel1     adr2----------------adr2+vel2 ako max(adr1,adr2) < min(adr1+vel1, adr2+vel2) onda ima presek
      if(max(spojeneSekcije[i->first].adresaPocetka, spojeneSekcije[j->first].adresaPocetka) < 
          min(spojeneSekcije[i->first].adresaPocetka + spojeneSekcije[i->first].velicina, 
              spojeneSekcije[j->first].adresaPocetka + spojeneSekcije[j->first].velicina))
      {
        cout << "GRESKA: Postoji preklapanje sekcija: " << i->first << " i " << j->first << endl;
        exit(-1);
      }
    }
  }
}

void Linker::mapirajSekcije(){
  // Prvo treba da mapiramo sekcije kojima je korisnik postavio adresu, zatim proverimo da li ima tu nekog preklapanja
  // Onda mapiramo i ostale sekcije pocev od najvise dostupne adrese(koja je ostala iza postavljenih sekcija)
  // Ovo je bitno da uradimo u redosledu navodjenja fajlova pa tako treba sortirati sekcije

  unsigned int prvaSlobodnaAdresa = 0;
  for(auto &elem : postavljeneSekcije){
    map<string, SpojenaSekcija>::iterator i = spojeneSekcije.find(elem.first);
    if(i == spojeneSekcije.end()){
      cout << "GRESKA: Naveli ste sekciju koja ne postoji: " << elem.first << endl;
      exit(-1);
    }
    spojeneSekcije[elem.first].adresaPocetka = elem.second;
    if(spojeneSekcije[elem.first].adresaPocetka + spojeneSekcije[elem.first].velicina > prvaSlobodnaAdresa){
      prvaSlobodnaAdresa = spojeneSekcije[elem.first].adresaPocetka + spojeneSekcije[elem.first].velicina;
    }
  }
  
  // Sad proverimo eventualno preklapanje:
  proveriPreklapanje(); // Ovo baca gresku ako ima preklapanja 

  // Sad treba mapirati ostale sekcije pocevsi od prve slobodne adrese
  vector<pair<string, SpojenaSekcija>> vec(spojeneSekcije.begin(), spojeneSekcije.end());
  sort(vec.begin(), vec.end(), [](pair<string, SpojenaSekcija> p1, pair<string, SpojenaSekcija> p2){
    return p1.second.idSpojeneSekcije < p2.second.idSpojeneSekcije;
  }); // Sortirali smo spojene sekcije rastuce po rednom broju 

  for(pair<string, SpojenaSekcija> i : vec){
    if(i.first == "UND") continue; // Preskacemo mapiranje nedefinisane sekcije 
    map<string, unsigned int>::iterator it = postavljeneSekcije.find(i.first);
    if(it != postavljeneSekcije.end()) continue; // Preksacemo mapirane sekcija koje su postavljene, njih smo vec obradili

    spojeneSekcije[i.first].adresaPocetka = prvaSlobodnaAdresa;
    prvaSlobodnaAdresa = spojeneSekcije[i.first].adresaPocetka+ i.second.velicina;
  }
}

void Linker::ispisiMapiraneSekcije(){
    fajl << "-------------Mapirane sekcije----------------" << endl;
    fajl << "Redni broj\t" << "Ime sekcije\t" << "Adresa[hex]\t" << "Velicina[hex]\t" << endl;
    vector<pair<string,SpojenaSekcija>> ts(spojeneSekcije.begin(), spojeneSekcije.end());
    for(pair<string,SpojenaSekcija> iter : ts){
      fajl << setw(10) << iter.second.idSpojeneSekcije << '\t';
      fajl << setw(11) << iter.first << '\t';
      fajl << setw(9) << hex <<  iter.second.adresaPocetka << '\t';
      fajl << setw(8) << hex << iter.second.velicina << endl;
      fajl << dec;
    } 
    fajl << endl << endl;
}

void Linker::spojiTabeleSimbola(){
  int redBr = 0;
  
  // Prvo dodajemo sekcije
  vector<pair<string, SpojenaSekcija>> vec(spojeneSekcije.begin(), spojeneSekcije.end());
  sort(vec.begin(), vec.end(), [](pair<string, SpojenaSekcija> p1, pair<string, SpojenaSekcija> p2){
    return p1.second.idSpojeneSekcije < p2.second.idSpojeneSekcije;
  }); // Sortirali smo spojene sekcije rastuce po rednom broju 
  for(pair<string, SpojenaSekcija> sek : vec){
    UlazUTabeluSimbola noviUlaz;
    noviUlaz.definisan = true;
    noviUlaz.eksteran = false;
    noviUlaz.globalan = false;
    noviUlaz.idSimbola = redBr++;
    noviUlaz.idSekcije = sek.second.idSpojeneSekcije;
    noviUlaz.vrednost = 0;
    krajnjaTabelaSimbola[sek.first] = noviUlaz; 
  }

  // Dodali smo sekcije, sad dodajemo simbole
  map<string, UlazUTabeluSimbola> eksterniSimboli; // Ovde pamtimo eksterne simbole

  for(string f : ulazniFajlovi){
    for(map<string, UlazUTabeluSimbola>::iterator i = tabeleSimbola[f].begin(); i != tabeleSimbola[f].end(); i++){
      
      map<string,SpojenaSekcija>::iterator src = spojeneSekcije.find(i->first);
      if(src != spojeneSekcije.end()) continue; // Ako je simbol sekcija preskacemo ga, vec smo ga dodali

      if(i->second.eksteran == true){ // Ako je simbol eksteran dodajemo ga u listu eksternih
        eksterniSimboli[i->first] = i->second;
      }
      else{
        if(i->second.globalan == true){ // Posto je globalan i nije eksteran dodajemo ga u tabelu, znamo da je definisan
          map<string, UlazUTabeluSimbola>::iterator src = krajnjaTabelaSimbola.find(i->first);
          if(src != krajnjaTabelaSimbola.end()){
            cout << "GRESKA: Visestruka definicija simbola: " << i->first << " ,iz fajla: " << f << endl;
            exit(-1);
          }
          i->second.idSimbola = redBr++;

          SpojenaSekcija sek = spojeneSekcije[tabeleSekcija[f][i->second.idSekcije].ime];
           
          i->second.idSekcije = sek.idSpojeneSekcije;
          // Vrednost simbola se uvecava za pocetnu adresu spojene sekcije kao i za ofset prvobitne sekcije u 
          // grupi sekcija istog imena 
          // Ovde mozda ne treba dodati adresu pocetka sekcije jer ako se fajl linkuje sa -relocatable
          // opcijom onda nam pocetne adrese sekcija nisu odredjene tj. sekcija moze poceti od 
          // proizvoljnog mesta
          i->second.vrednost = i->second.vrednost /* + sek.adresaPocetka*/ + sek.sekcijeKojeJeCine[f];
          
          krajnjaTabelaSimbola[i->first] = i->second;
        }
      }

    }
  }
  // Probamo da razresimo eksterne simbole
  for(map<string, UlazUTabeluSimbola>::iterator i = eksterniSimboli.begin(); i != eksterniSimboli.end(); i++){
    map<string, UlazUTabeluSimbola>::iterator src = krajnjaTabelaSimbola.find(i->first);
    if(src == krajnjaTabelaSimbola.end()){
      // simbol je eksteran a nema ga u tabeli 
      if(this->isHex){ // Ako treba da generisemo izvrsni fajl ovo je greska
        cout << "Nije definisan eksterni simbol: " << i->first << endl;
        exit(-1);
      }
      else{ // A ako je u pitanju relokativni fajl onda jednostavno dodamo simbol kao eksterni
        i->second.idSimbola = redBr++;
        i->second.idSekcije = 0;
        i->second.eksteran = true;
        krajnjaTabelaSimbola[i->first] = i->second;
      }
    }
  }

}

void Linker::ispisiKrajnjuTabeluSimbola(){
  fajl << "--------------------------Krajnja tabela simbola-----------------------------" << endl;
  fajl << "Redni broj\t" << setw(15) << "Ime simbola\t" << "Vrednost[hex]\t" << "Tip\t" << setw(10) << "ID sekcije\t" << "Vezivanje\t" << "Definisan\n";
  vector<pair<string,UlazUTabeluSimbola>> ts(krajnjaTabelaSimbola.begin(), krajnjaTabelaSimbola.end());
  // Sortiramo simbole po njihovom id-ju
  sort(ts.begin(), ts.end(), [](pair<string, UlazUTabeluSimbola> u1, pair<string, UlazUTabeluSimbola> u2){
    return u1.second.idSimbola < u2.second.idSimbola;
  });
  for(pair<string,UlazUTabeluSimbola> iter : ts){

    fajl << setw(10) << iter.second.idSimbola << '\t';
    fajl << setw(15) << iter.first << '\t';
    fajl << setw(8) << hex << iter.second.vrednost << dec << '\t';
    auto it = spojeneSekcije.find(iter.first);
    bool sekcija = (it != spojeneSekcije.end());
    fajl << setw(6) << (sekcija ? "SCTN" : "NOTYPE") << '\t';
    fajl << setw(10) << iter.second.idSekcije << '\t';
    fajl << setw(9) << ((iter.second.globalan == true || iter.second.eksteran == true) ? "GLOB" : "LOC" )<< '\t';
    fajl << setw(9) << (iter.second.definisan == true ? "DA" : "NE") << endl;
  }
  fajl << endl << endl;
}

void Linker::spojiRelokacioneTabele(){
  for(string f : ulazniFajlovi){
    for(map<string, vector<UlazURelokacionuTabelu>>::iterator i = tabeleRelokacija[f].begin(); i != tabeleRelokacija[f].end(); i++){
      for(UlazURelokacionuTabelu u : i->second){
        
        UlazURelokacionuTabelu noviUlaz;
        auto src = spojeneSekcije.find(u.simbol);
        // Ako je u ulazu sekcija, treba joj dodati ofset od pocetka istoimenih sekcija
        if(src != spojeneSekcije.end()){
          noviUlaz.addend = u.addend + spojeneSekcije[u.simbol].sekcijeKojeJeCine[f];
        }
        else{
          noviUlaz.addend = u.addend;
        }
        noviUlaz.simbol = u.simbol;
        noviUlaz.offset = u.offset + spojeneSekcije[i->first].sekcijeKojeJeCine[f];
        krajnjaTabeleRelokacija[i->first].push_back(noviUlaz);
      }
    }
  }
}

void Linker::ispisiKrajnjuRelokacionuTabelu(){
    fajl << "-------------------------Krajnji relokacioni zapisi-----------------------" << endl;

    for(map<string, SpojenaSekcija>::iterator iter = spojeneSekcije.begin(); iter != spojeneSekcije.end(); iter++){
    
      fajl << "Tabela realokacija sekcije: " << iter->first << endl;
    
      map<string, vector<UlazURelokacionuTabelu>>::iterator i2 = krajnjaTabeleRelokacija.find(iter->first);
    
      if(i2 == krajnjaTabeleRelokacija.end()){
        fajl << "Za ovu sekciju ne postoje relokacioni zapisi\n\n";
      }
      else{
        fajl << "Offset[hex]\t" << "Ime simbola\t" << setw(8) << setfill(' ') << "Tip\t" << "Addend[hex]" << endl;
        for(int i = 0; i < i2->second.size(); i++){
          fajl << hex << setfill('0') << setw(8) << hex << i2->second.at(i).offset << dec << '\t';
          
          fajl << setw(12)<< setfill(' ') << i2->second.at(i).simbol << '\t';
          fajl << setw(8) << "R_X32_32 ";
          fajl << hex << setfill('0') << setw(8) << hex << i2->second.at(i).addend << dec << endl << endl;
        }
      }
  }

  fajl << endl << endl;
}

void Linker::spojiKodoveSekcija(){
  for(string f : ulazniFajlovi){
    for(map<string, stringstream*>::iterator i = kodovi[f].begin(); i != kodovi[f].end(); i++){
      auto src = krajnjaTabelaKodova.find(i->first);
      if(src == krajnjaTabelaKodova.end()){
        krajnjaTabelaKodova[i->first] = new stringstream();
      }
      string kod = i->second->str();
      krajnjaTabelaKodova[i->first]->write((char*)kod.c_str(), kod.size());
    }
  }
}

void Linker::ispisiKrajnjuTabeluKodova(){
  fajl << "-----------------------Sadrzaji sekcija----------------------------\n";
  
    for(map<string, SpojenaSekcija>::iterator iter = spojeneSekcije.begin(); iter != spojeneSekcije.end(); iter++){
      fajl << "------------Sekcija: " << iter->first <<  "---------------" << endl;
      fajl << dec;
      if(iter->second.velicina == 0){
        fajl << "Sekcija nema podataka" << endl << endl;
      }
      else{
        fajl << "Velicina sekcije[hex]: " << hex << iter->second.velicina << dec << endl;
        int brRedova = (iter->second.velicina + 7) / 8 ;

        for(int i = 0; i < brRedova; i++){
          fajl << hex << setfill('0') << setw(4) << (0xffff & i*8) << ": ";
          for(int j = i*8; j < (i+1)*8; j++){
            char c;
            if(j < iter->second.velicina){
              krajnjaTabelaKodova[iter->first]->read(&c, sizeof(char));
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
}

void Linker::napraviKrajnjuTabeluSekcija(){
  for(auto& ulaz : spojeneSekcije){
    UlazUTabeluSekcija noviUlaz;
    noviUlaz.idSekcije = ulaz.second.idSpojeneSekcije;
    noviUlaz.velicina = ulaz.second.velicina;
    noviUlaz.adresaPocetka = ulaz.second.adresaPocetka;
    noviUlaz.ime = ulaz.first;
    krajnjaTabelaSekcija[noviUlaz.idSekcije] = noviUlaz;
  }
}


void Linker::razresiRelokacioneZapise(){ // Ovo se koristi ako je za linkovanje uzeta opcija -hex

  // Prvo svim simbolimo treba dodati adresu pocetka sekcije na njihovu vrednost
  for(map<string, UlazUTabeluSimbola>::iterator i = krajnjaTabelaSimbola.begin(); i != krajnjaTabelaSimbola.end(); i++){
    i->second.vrednost += krajnjaTabelaSekcija[i->second.idSekcije].adresaPocetka;
  }
  // Sada idemo po relokacionim zapisima i krpimo
  for(auto &relZapis : krajnjaTabeleRelokacija){
    for(UlazURelokacionuTabelu ulaz : relZapis.second){
      // Za sekcije ovo ce biti njihova pocetna adresa za simbole to ce biti njihova vrednost unutar sekcije
      // uvecana za adresu pocetka sekcije, to smo pripremili u prvoj for petlji ove funkcije
      unsigned int vrednostSimbola = krajnjaTabelaSimbola[ulaz.simbol].vrednost;
      vrednostSimbola += ulaz.addend; // Ovo treba da upisemo na offset
      krajnjaTabelaKodova[relZapis.first]->seekp(ulaz.offset); // Postavimo kurzor na mesto prepravke
      krajnjaTabelaKodova[relZapis.first]->write((char*)&vrednostSimbola, sizeof(unsigned int));
      krajnjaTabelaKodova[relZapis.first]->seekp(0, std::ios_base::end); // Vratimo kurzor na kraj koda
    }
  }

}

void Linker::napraviBinarniFajl(){
  ofstream binarniFajl(izlazniFajl, ios::out | ios::binary); 

  if(isHex){ // Ako je odabrana -hex opcija u binarni fajl ispisujemo samo kodove sekcija, ispisacemo i pocetnu adresu sekcije kao i velicinu sekcije
    int brSekcija = krajnjaTabelaSekcija.size() - 1; // Jer hocemo da izostavimo und sekciju
    binarniFajl.write((char*)&brSekcija, sizeof(int));
    for(auto &sekcija : krajnjaTabelaSekcija){
      if(sekcija.second.ime == "UND") continue; 
      binarniFajl.write((char*)&sekcija.second.adresaPocetka, sizeof(sekcija.second.adresaPocetka));
      string kod = krajnjaTabelaKodova[sekcija.second.ime]->str();
      binarniFajl.write((char*)&sekcija.second.velicina,sizeof(int));
      binarniFajl.write((char*)kod.c_str(), sekcija.second.velicina);
    }

  }
  else{ // Ako je odabrana opcija -relocatable radimo slicnu stvar kao sto smo radili sa asemblerom, vecina koda ce biti iskopirana odatle

    // Ispisemo tabelu sekcija
    int brUlaza = krajnjaTabelaSekcija.size();
    binarniFajl.write((char*)(&brUlaza), sizeof(int));
    for(map<int,UlazUTabeluSekcija>::iterator i = krajnjaTabelaSekcija.begin(); i != krajnjaTabelaSekcija.end(); i++){

      // Prvo ime sekcije
      string ime = i->second.ime;
      int duzina = ime.length();
      binarniFajl.write((char*)&duzina, sizeof(int));
      binarniFajl.write(ime.c_str(), duzina);

      // Ostale vrednosti
      binarniFajl.write((char*)(&i->second.idSekcije), sizeof(i->second.idSekcije));
      binarniFajl.write((char*)(&i->second.velicina), sizeof(i->second.velicina));

    }

    // Ispisemo tabelu simbola
    brUlaza = krajnjaTabelaSimbola.size();
    binarniFajl.write((char*)&brUlaza, sizeof(int));
    for(map<string, UlazUTabeluSimbola>::iterator i = krajnjaTabelaSimbola.begin(); i != krajnjaTabelaSimbola.end(); i++){
      // Prvo idemo kljuc
      string ime = i->first;
      int duzina = ime.length();
      binarniFajl.write((char*)&duzina, sizeof(int));
      binarniFajl.write(ime.c_str(), duzina);

      // Sada idu ostali atributi
      binarniFajl.write((char*)(&i->second.definisan), sizeof(i->second.definisan));
      binarniFajl.write((char*)(&i->second.globalan), sizeof(i->second.globalan));
      binarniFajl.write((char*)(&i->second.idSimbola), sizeof(i->second.idSimbola));
      binarniFajl.write((char*)(&i->second.eksteran), sizeof(i->second.eksteran));
      binarniFajl.write((char*)(&i->second.vrednost), sizeof(i->second.vrednost));

      binarniFajl.write((char*)(&i->second.idSekcije), sizeof(int));
    }

    // Ispisujemo relokacione tabele
    brUlaza = krajnjaTabeleRelokacija.size();
    binarniFajl.write((char*)&brUlaza, sizeof(int));
    for(map<string, vector<UlazURelokacionuTabelu>>::iterator i = krajnjaTabeleRelokacija.begin(); i != krajnjaTabeleRelokacija.end(); i++){
      // prvo kljuc, ime sekcije za koju je namenjen relokacioni zapis
      string ime = i->first;
      int duzina = ime.length();
      binarniFajl.write((char*)&duzina, sizeof(int));
      binarniFajl.write(ime.c_str(), duzina);

      // Ostale vrednosti
      int brRedova = i->second.size();
      binarniFajl.write((char*)&brRedova, sizeof(int));
      for(int j = 0; j < brRedova; j++){
        binarniFajl.write((char*)(&i->second.at(j).addend), sizeof(int));
        binarniFajl.write((char*)(&i->second.at(j).offset), sizeof(int));

        int duzina = i->second.at(j).simbol.length();
        binarniFajl.write((char*)(&duzina), sizeof(int));
        binarniFajl.write(i->second.at(j).simbol.c_str(), duzina);
      }
    }

    // upisujemo kod svih sekcija
    brUlaza = krajnjaTabelaKodova.size();
    binarniFajl.write((char*)&brUlaza, sizeof(int));
    for(map<string,stringstream*>::iterator i = krajnjaTabelaKodova.begin(); i != krajnjaTabelaKodova.end(); i++){
      // prvo kljuc, pretstavlja ime sekcije ciji kod upsujemo u fajl
      string ime = i->first;
      int duzina = ime.length();
      binarniFajl.write((char*)&duzina, sizeof(int));
      binarniFajl.write(ime.c_str(), duzina);

      // Sam kod

      i->second->seekg(0); // Postavimo poziciju za citanje od pocetka 
      string kod = i->second->str();
      duzina = kod.length();
      binarniFajl.write((char*)&duzina, sizeof(int));
      binarniFajl.write(kod.data(), duzina); // Proveri ovo, mozda je bolja opcija kod.c_str(), stavljamo data() da nam ne bi stavio '\0' na kraj stringa
    }

  }
  binarniFajl.close();
}

void Linker::linkuj(){

  dohvatiPodatke(); // Prvo dohvarimo podatke iz binarnih fajlova

  spojiSekcijeIstogImena();

  if(isHex){
    mapirajSekcije(); // Ovo se poziva samo za -hex
  }

  spojiTabeleSimbola(); 

  spojiRelokacioneTabele();

  spojiKodoveSekcija();

  napraviKrajnjuTabeluSekcija(); // Ova tabela je vise pomocna nego sto stvarno treba

  if(isHex){
    razresiRelokacioneZapise();  // Ovo bi trebalo da se poziva samo za -hex opciju 
  }

  napraviBinarniFajl();
  ispisiMapiraneSekcije();
  ispisiKrajnjuTabeluSimbola();
  ispisiKrajnjuRelokacionuTabelu();
  ispisiKrajnjuTabeluKodova();
  cout << "Linkovanje zavrseno.\n";
}