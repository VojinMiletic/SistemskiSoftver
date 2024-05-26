
g++ -g ./src/mainLinker.cpp ./src/Linker.cpp -o linker

if [ $? -eq 0 ]; then
  echo "Uspeh. Izvrsna datoteka: linker"
else
  echo "Greska prilikom kompajliranja"
fi