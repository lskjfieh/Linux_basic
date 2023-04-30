#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int g_unval;
int g_val = 100;
//int main(int argc,char *argv[], char *env[]){
int main(){
  const  char *s = "hello world";
  printf("code addr: %p\n", main);
  printf("string rdonly addr: %p\n", s);
  printf("uninit addr: %p\n", &g_unval);
  printf("init addr: %p\n", &g_val);
  char *heap = (char*)malloc(10);

  printf("heap addr: %p\n", heap);

  printf("stack addr: %p\n", &s);
  printf("stack addr: %p\n", &heap);

  int a = 10;
  int b = 30;
  printf("stack addr: %p\n", &a);
  printf("stack addr: %p\n", &b);
 // printf("I am a process pid: %d, ppid: %d", getpid(), getppid());
 // printf("PATH: %s\n", getenv("PATH"));
 // printf("HOME: %s\n", getenv("HOME"));
 // printf("SHELL: %s\n", getenv("SHELL"));
 //extern char **environ;
 //for(int i = 0; environ[i]; i++){
 //printf("%d->%s\n", i, environ[i]);
 //}
 // if(argc != 2){
 //   printf("Usage: %s -[a|h]\n", argv[0]);
 //   return 1;
 // }
  //为什么？
 // if(strcmp(argv[1], "-h") == 0){
 //   printf("hello today !\n");
 // }
 // else if(strcmp(argv[1], "-a") == 0){
 //   printf("hello everyday\n");
 // }
 // else{
 //   printf("hello wprld!\n");
 //}
  return 0;
}
