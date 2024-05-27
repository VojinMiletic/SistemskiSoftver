
g++ -g ./src/mainEmulator.cpp ./src/Emulator.cpp -o emulator

if [ $? -eq 0 ]; then
  echo "Uspeh. Izvrsna datoteka: emulator"
else
  echo "Greska prilikom kompajliranja"
fi