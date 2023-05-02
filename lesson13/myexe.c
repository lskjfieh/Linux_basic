#include <stdio.h>
#include <unistd.h>
int main(){
  extern char**environ;
  for(int i = 0; environ[i]; i++){
    printf("%s", environ[i]);
  }
  printf("my exe running ... done\n");
  return 0;
}
