#include "../inc/helper.hpp"


vector<Leksicki_Element*>* elementi_help = nullptr;
struct argument* argumenti_help;
FILE* fajl = nullptr;

struct argument* napraviArgument(int br, char* sym){
  struct argument* arg = (argument*) malloc(sizeof(struct argument));
  arg->broj = br;
  arg->simbol = sym;
  return arg;
}

void freeArgument(struct argument* args){
  if(!args) return ;
  while(args){
    struct argument* old = args;
    args = args->next;
    free(old);
  }
  argumenti_help = nullptr;
}

char* kopiraj_string(const char* str){
  size_t len = strnlen(str, 64);
  char *ret = (char*)malloc(len+1);
  strncpy(ret,str,len);
  ret[len] = '\0';
  return ret;
}