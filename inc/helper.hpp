#include <vector>
#include <string.h>
#include <stdlib.h>

#include "Direktiva.hpp"
#include "Instrukcija.hpp"

struct argument{
  int broj;
  char* simbol;
  struct argument* next;
};

extern struct argument* argumenti_help;


struct argument* napraviArgument(int, char*);

void freeArgument(struct argument* args);

char* kopiraj_string(const char*);