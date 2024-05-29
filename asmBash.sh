
g++ -g ./src/Direktiva.cpp ./src/Instrukcija.cpp ./src/helper.cpp ./src/LeksickiElement.cpp ./src/Operand.cpp ./src/Asembler.cpp ./src/mainAsembler.cpp ./misc/parser.cpp ./misc/lexer.cpp -o asembler

if [ $? -eq 0 ]; then
  echo "Uspeh. Izvrsna datoteka: asembler"
else
  echo "Greska prilikom kompajliranja"
fi